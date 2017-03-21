/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "include/m2mcallbackstorage.h"
#include "include/m2mreporthandler.h"
#include "include/nsdllinker.h"
#include "mbed-client/m2mblockmessage.h"
#include "mbed-trace/mbed_trace.h"

#define TRACE_GROUP "mClt"

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MResourceBase(res_name,
          resource_type,
          type,
          path,
          external_blockwise_store,
          multiple_instance
          ),
 _parent_resource(parent)
{
    M2MBase::set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MResourceBase(res_name,
          resource_type,
          type,
          path,
          external_blockwise_store,
          multiple_instance),
 _parent_resource(parent)
{
    // XXX: isn't the base type wrong?
    M2MBase::set_base_type(M2MBase::Resource);
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const lwm2m_parameters_s* s,
                                         M2MBase::DataType type)
: M2MResourceBase(s, type),
  _parent_resource(parent)
{
    //TBD: put to flash, or parse from the uri_path!!!!
    //same for the _object_instance_id.
    //M2MBase::set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::~M2MResourceInstance()
{
}


bool M2MResourceInstance::handle_observation_attribute(const char *query)
{
    tr_debug("M2MResourceInstance::handle_observation_attribute - is_under_observation(%d)", is_under_observation());
    bool success = false;

    M2MReportHandler *handler = M2MBase::report_handler();
    if (!handler) {
        handler = M2MBase::create_report_handler();
    }

    if (handler) {
        success = handler->parse_notification_attribute(query,
                M2MBase::base_type(), resource_instance_type());
        if(success) {
            if (is_under_observation()) {
                handler->set_under_observation(true);
            }
        } else {
            handler->set_default_values();
        }
    }
    return success;
}





sn_coap_hdr_s* M2MResourceInstance::handle_get_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler)
{
    tr_debug("M2MResourceInstance::handle_get_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CONTENT;
    sn_coap_hdr_s *coap_response = sn_nsdl_build_response(nsdl,
                                                          received_coap_header,
                                                          msg_code);
    if(received_coap_header) {
        // process the GET if we have registered a callback for it
        if ((operation() & SN_GRS_GET_ALLOWED) != 0) {
            if(coap_response) {
                if(resource_instance_type() == M2MResourceInstance::OPAQUE) {
                    coap_response->content_format = sn_coap_content_format_e(COAP_CONTENT_OMA_OPAQUE_TYPE);
                } else {
                    coap_response->content_format = sn_coap_content_format_e(0);
                }
                // fill in the CoAP response payload
                coap_response->payload_ptr = NULL;
                uint32_t payload_len = 0;
#ifndef DISABLE_BLOCK_MESSAGE
                //If handler exists it means that resource value is stored in application side
                if (block_message() && block_message()->is_block_message()) {
                    outgoing_block_message_callback* outgoing_block_message_cb = (outgoing_block_message_callback*)M2MCallbackStorage::get_callback(*this,
                                                                                                        M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
                    if (outgoing_block_message_cb) {
                        String name = "";
                        if (received_coap_header->uri_path_ptr != NULL &&
                                received_coap_header->uri_path_len > 0) {
                            name.append_raw((char *)received_coap_header->uri_path_ptr,
                                             received_coap_header->uri_path_len);
                        }
                        (*outgoing_block_message_cb)(name, coap_response->payload_ptr, payload_len);
                    }
                } else {
#endif
                    get_value(coap_response->payload_ptr,payload_len);
#ifndef DISABLE_BLOCK_MESSAGE
                }
#endif

                coap_response->payload_len = payload_len;
                coap_response->options_list_ptr = sn_nsdl_alloc_options_list(nsdl, coap_response);

                coap_response->options_list_ptr->max_age = max_age();

                if(received_coap_header->options_list_ptr) {
                    if(received_coap_header->options_list_ptr->observe != -1) {
                        if (is_observable()) {
                            uint32_t number = 0;
                            uint8_t observe_option = 0;
                            observe_option = received_coap_header->options_list_ptr->observe;

                            if(START_OBSERVATION == observe_option) {
                                tr_debug("M2MResourceInstance::handle_get_request - Starts Observation");
                                // If the observe length is 0 means register for observation.
                                if(received_coap_header->options_list_ptr->observe != -1) {
                                    number = received_coap_header->options_list_ptr->observe;
                                }
                                if(received_coap_header->token_ptr) {
                                    tr_debug("M2MResourceInstance::handle_get_request - Sets Observation Token to resource");
                                    set_observation_token(received_coap_header->token_ptr,
                                                          received_coap_header->token_len);
                                }
                                // If the observe value is 0 means register for observation.
                                if(number == 0) {
                                    tr_debug("M2MResourceInstance::handle_get_request - Put Resource under Observation");
                                    set_under_observation(true,observation_handler);
                                    M2MBase::add_observation_level(M2MBase::R_Attribute);
                                    coap_response->options_list_ptr->observe = observation_number();
                                }
                            } else if (STOP_OBSERVATION == observe_option) {
                                tr_debug("M2MResourceInstance::handle_get_request - Stops Observation");
                                set_under_observation(false,NULL);
                                M2MBase::remove_observation_level(M2MBase::R_Attribute);
                            }
                        } else {
                            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
                        }
                    }
                }
            }
        }else {
            tr_error("M2MResourceInstance::handle_get_request - Return COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
            // Operation is not allowed.
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
        }
    } else {
        msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    }
    if(coap_response) {
        coap_response->msg_code = msg_code;
    }
    return coap_response;
}

sn_coap_hdr_s* M2MResourceInstance::handle_put_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler,
                                               bool &execute_value_updated)
{
    tr_debug("M2MResourceInstance::handle_put_request()");


        sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
        sn_coap_hdr_s *coap_response = sn_nsdl_build_response(nsdl,
                                                               received_coap_header,
                                                               msg_code);
        // process the PUT if we have registered a callback for it
        if(received_coap_header && coap_response) {
            uint16_t coap_content_type = 0;
            if(received_coap_header->content_format != COAP_CT_NONE) {
                coap_content_type = received_coap_header->content_format;
            }
            if(received_coap_header->options_list_ptr &&
               received_coap_header->options_list_ptr->uri_query_ptr) {
                char *query = (char*)alloc_string_copy(received_coap_header->options_list_ptr->uri_query_ptr,
                                                        received_coap_header->options_list_ptr->uri_query_len);
                if (query){
                    tr_debug("M2MResourceInstance::handle_put_request() - Query %s", query);

                    // if anything was updated, re-initialize the stored notification attributes
                    if (!handle_observation_attribute(query)){
                        tr_debug("M2MResourceInstance::handle_put_request() - Invalid query");
                        msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                    }
                    free(query);
                }
            } else if ((operation() & SN_GRS_PUT_ALLOWED) != 0) {
                tr_debug("M2MResourceInstance::handle_put_request() - Request Content-Type %d", coap_content_type);

                if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type) {
                    msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                } else {
                    bool external_block_store = false;
#ifndef DISABLE_BLOCK_MESSAGE
                    if (block_message()) {
                        block_message()->set_message_info(received_coap_header);
                        if (block_message()->is_block_message()) {
                            external_block_store = true;
                            incoming_block_message_callback* incoming_block_message_cb = (incoming_block_message_callback*)M2MCallbackStorage::get_callback(*this,
                                                                                    M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
                            if (incoming_block_message_cb) {
                                (*incoming_block_message_cb)(_block_message_data);
                            }
                            if (block_message()->is_last_block()) {
                                block_message()->clear_values();
                                coap_response->coap_status = COAP_STATUS_PARSER_BLOCKWISE_MSG_RECEIVED;
                            } else {
                                coap_response->coap_status = COAP_STATUS_PARSER_BLOCKWISE_MSG_RECEIVING;
                            }
                            if (block_message()->error_code() != M2MBlockMessage::ErrorNone) {
                                block_message()->clear_values();
                            }
                        }
                    }
#endif
                    if (!external_block_store) {
                        set_value(received_coap_header->payload_ptr, received_coap_header->payload_len);
                    }
                    if(received_coap_header->payload_ptr) {
                       tr_debug("M2MResourceInstance::handle_put_request() - Update Resource with new values");
                        if(observation_handler) {
                            String value = "";
                            if (received_coap_header->uri_path_ptr != NULL &&
                                received_coap_header->uri_path_len > 0) {
                                value.append_raw((char*)received_coap_header->uri_path_ptr, received_coap_header->uri_path_len);
                            }
                            execute_value_updated = true;
                        }
                    }
                }
            } else {
                // Operation is not allowed.
                tr_error("M2MResourceInstance::handle_put_request() - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
            }
        } else {
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
        }
        if(coap_response) {
            coap_response->msg_code = msg_code;
        }

    return coap_response;
}

uint16_t M2MResourceInstance::object_instance_id() const
{
    const M2MObjectInstance& parent_object_instance = get_parent_resource().get_parent_object_instance();
    return parent_object_instance.instance_id();
}


M2MResource& M2MResourceInstance::get_parent_resource() const
{
    return _parent_resource;
}

const char* M2MResourceInstance::object_name() const
{
    const M2MObjectInstance& parent_object_instance = _parent_resource.get_parent_object_instance();
    const M2MObject& parent_object = parent_object_instance.get_parent_object();

    return parent_object.name();
}

