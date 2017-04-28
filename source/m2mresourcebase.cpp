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
#include "mbed-client/m2mresourcebase.h"
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

M2MResourceBase::M2MResourceBase(
                                         const String &res_name,
                                         M2MBase::Mode resource_mode,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MBase(res_name,
          resource_mode,
#ifndef DISABLE_RESOURCE_TYPE
          resource_type,
#endif
          path,
          external_blockwise_store,
          multiple_instance,
          type)
#ifndef DISABLE_BLOCK_MESSAGE
 ,_block_message_data(NULL)
#endif
{
}

M2MResourceBase::M2MResourceBase(
                                         const String &res_name,
                                         M2MBase::Mode resource_mode,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MBase(res_name,
          resource_mode,
#ifndef DISABLE_RESOURCE_TYPE
          resource_type,
#endif
          path,
          external_blockwise_store,
          multiple_instance,
          type)
#ifndef DISABLE_BLOCK_MESSAGE
 ,_block_message_data(NULL)
#endif
{
    M2MBase::set_base_type(M2MBase::ResourceInstance);
    if( value != NULL && value_length > 0 ) {
        sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
        res->resource = alloc_string_copy(value, value_length);
        res->resourcelen = value_length;
    }
}

M2MResourceBase::M2MResourceBase(
                                         const lwm2m_parameters_s* s,
                                         M2MBase::DataType /*type*/)
: M2MBase(s)
#ifndef DISABLE_BLOCK_MESSAGE
  ,_block_message_data(NULL)
#endif
{
    // we are not there yet for this check as this is called from M2MResource(): assert(base_type() == M2MBase::ResourceInstance);
}

M2MResourceBase::~M2MResourceBase()
{
    execute_callback* callback = (execute_callback*)M2MCallbackStorage::remove_callback(*this,
                                    M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);
    delete callback;

    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);
#ifndef DISABLE_BLOCK_MESSAGE
    incoming_block_message_callback *in_callback = (incoming_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
    delete in_callback;

    outgoing_block_message_callback *out_callback = (outgoing_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
    delete out_callback;
#endif

    notification_sent_callback *notif_callback = (notification_sent_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
    delete notif_callback;

    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);

#ifndef DISABLE_BLOCK_MESSAGE
    delete _block_message_data;
#endif
}

M2MResourceBase::ResourceType M2MResourceBase::resource_instance_type() const
{
    M2MBase::lwm2m_parameters_s* param = M2MBase::get_lwm2m_parameters();
    M2MBase::DataType type = param->data_type;
    return convert_data_type(type);
}


bool M2MResourceBase::set_execute_function(execute_callback callback)
{
    execute_callback* old_callback = (execute_callback*)M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);
    delete old_callback;
    // XXX: create a copy of the copy of callback object. Perhaps it would better to
    // give a reference as parameter and just store that, as it would save some memory.
    execute_callback* new_callback = new execute_callback(callback);

    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);
}

bool M2MResourceBase::set_execute_function(execute_callback_2 callback)
{
    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);

    return M2MCallbackStorage::add_callback(*this, (void*)callback, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);
}

void M2MResourceBase::clear_value()
{
    tr_debug("M2MResourceBase::clear_value");
    sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
    free(res->resource);
    res->resource = NULL;
    res->resourcelen = 0;

    report();
}

bool M2MResourceBase::set_value(int64_t value)
{
    bool success;
    // max len of "-9223372036854775808" plus zero termination
    char buffer[20+1];
    uint32_t size = m2m::itoa_c(value, buffer);

    success = set_value((const uint8_t*)buffer, size);

    return success;
}

bool M2MResourceBase::set_value(const uint8_t *value,
                                    const uint32_t value_length)
{
    tr_debug("M2MResourceBase::set_value()");
    bool success = false;
    bool value_changed = false;
    if(is_value_changed(value,value_length)) {
        value_changed = true;
    }
    if( value != NULL && value_length > 0 ) {
        success = true;

        sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
        free(res->resource);
        res->resource = NULL;
        res->resourcelen = 0;

        res->resource = alloc_string_copy(value, value_length);
        if(res->resource) {
            res->resourcelen = value_length;
            if( value_changed ) { //
                if (resource_instance_type() == M2MResourceBase::STRING) {
                    M2MReportHandler *report_handler = M2MBase::report_handler();
                    if(report_handler && is_under_observation()) {
                        report_handler->set_notification_trigger();
                    }
                }
                else {
                    report();
                }
            }
        }
    }
    return success;
}

void M2MResourceBase::report()
{
    M2MBase::Observation observation_level = M2MBase::observation_level();
    tr_debug("M2MResourceBase::report() - level %d", observation_level);

    // We must combine the parent object/objectinstance/resource observation information
    // when determining if there is observation set or not.
    M2MObjectInstance& object_instance = get_parent_resource().get_parent_object_instance();
    int parent_observation_level = (int)object_instance.observation_level();

    parent_observation_level |= (int)object_instance.get_parent_object().observation_level();
    parent_observation_level |= (int)get_parent_resource().observation_level();
    parent_observation_level |= (int)observation_level;

    tr_debug("M2MResourceInstance::report() - combined level %d", parent_observation_level);

    if((M2MBase::O_Attribute & parent_observation_level) == M2MBase::O_Attribute ||
       (M2MBase::OI_Attribute & parent_observation_level) == M2MBase::OI_Attribute) {
        tr_debug("M2MResourceBase::report() -- object/instance level");
        M2MObjectInstance& object_instance = get_parent_resource().get_parent_object_instance();
        object_instance.notification_update((M2MBase::Observation)parent_observation_level);
    }

    if(M2MBase::Dynamic == mode() &&
       (M2MBase::R_Attribute & parent_observation_level) == M2MBase::R_Attribute) {
        tr_debug("M2MResourceBase::report() - resource level");

        if ((resource_instance_type() != M2MResourceInstance::STRING) && (observation_level != M2MBase::None)) {
            M2MReportHandler *report_handler = M2MBase::report_handler();
            if (report_handler && is_observable()) {
                sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();    
                if(res->resource) {
                    report_handler->set_value(atof((const char*)res->resource));
                } else {
                    report_handler->set_value(0);
                }
            }
        }
        else {
            if (base_type() == M2MBase::ResourceInstance) {
                const M2MResource& parent_resource = get_parent_resource();
                M2MReportHandler *report_handler = parent_resource.report_handler();
                if(report_handler && parent_resource.is_observable()) {
                    report_handler->set_notification_trigger(parent_resource.get_parent_object_instance().instance_id());
                }
            }
        }
    } else if(M2MBase::Static == mode()) {
        M2MObservationHandler *obs_handler = observation_handler();
        if(obs_handler) {
            obs_handler->value_updated(this);
        }
    } else {
        tr_debug("M2MResourceBase::report() - mode = %d, is_observable = %d", mode(), is_observable());
    }
}

bool M2MResourceBase::is_value_changed(const uint8_t* value, const uint32_t value_len)
{
    bool changed = false;
    sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();

    if(value_len != res->resourcelen) {
        changed = true;
    } else if(value && !res->resource) {
        changed = true;
    } else if(res->resource && !value) {
        changed = true;
    } else {
        if (res->resource) {
            if (memcmp(value, res->resource, res->resourcelen) != 0) {
                changed = true;
            }
        }
    }
    tr_debug("M2MResourceBase::is_value_changed() -- %s", changed ? "true" : "false");
    return changed;
}

void M2MResourceBase::execute(void *arguments)
{
    // XXX: this line is expected by seven testcases and until this code hits master branch
    // the testcases can not be modified and we need to print the false information too.
    tr_debug("M2MResourceInstance::execute");

    execute_callback* callback = (execute_callback*)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);

    if (callback) {
        (*callback)(arguments);
    }

    execute_callback_2 callback2 = (execute_callback_2)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);
    if (callback2) {
        (*callback2)(arguments);
    }
}

void M2MResourceBase::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
    if(res->resource && res->resourcelen > 0) {
        value = alloc_string_copy(res->resource, res->resourcelen);
        if(value) {
            value_length = res->resourcelen;
        }
    }
}

int64_t M2MResourceBase::get_value_int() const
{
    int64_t value_int = 0;

    // XXX: this relies on having a zero terminated value?!
    const char *value_string = (char *)value();
    if (value_string) {
        value_int = atoll(value_string);
    }
    return value_int;
}

String M2MResourceBase::get_value_string() const
{
    // XXX: do a better constructor to avoid pointless malloc
    String value;
    if (get_nsdl_resource()->resource) {
        value.append_raw((char*)get_nsdl_resource()->resource, get_nsdl_resource()->resourcelen);
    }
    return value;
}

uint8_t* M2MResourceBase::value() const
{
    return get_nsdl_resource()->resource;
}

uint32_t M2MResourceBase::value_length() const
{   
    return get_nsdl_resource()->resourcelen;
}

sn_coap_hdr_s* M2MResourceBase::handle_get_request(nsdl_s *nsdl,
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

                                // If the observe value is 0 means register for observation.
                                if(number == 0) {
                                    tr_debug("M2MResourceInstance::handle_get_request - Put Resource under Observation");
                                    set_under_observation(true,observation_handler);
                                    M2MBase::add_observation_level(M2MBase::R_Attribute);
                                    coap_response->options_list_ptr->observe = observation_number();
                                }

                                if(received_coap_header->token_ptr) {
                                    tr_debug("M2MResourceInstance::handle_get_request - Sets Observation Token to resource");
                                    set_observation_token(received_coap_header->token_ptr,
                                                          received_coap_header->token_len);
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

sn_coap_hdr_s* M2MResourceBase::handle_put_request(nsdl_s *nsdl,
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


#ifndef DISABLE_BLOCK_MESSAGE

M2MBlockMessage* M2MResourceBase::block_message() const
{
    return _block_message_data;
}

bool M2MResourceBase::set_incoming_block_message_callback(incoming_block_message_callback callback)
{
    incoming_block_message_callback* old_callback = (incoming_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
    delete old_callback;

    // copy the callback object. This will change on next version to be a direct pointer to a interface class,
    // this FPn<> is just too heavy for this usage.
    incoming_block_message_callback* new_callback = new incoming_block_message_callback(callback);

    delete _block_message_data;
    _block_message_data = NULL;
    _block_message_data = new M2MBlockMessage();

    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
}

bool M2MResourceBase::set_outgoing_block_message_callback(outgoing_block_message_callback callback)
{
    outgoing_block_message_callback *old_callback = (outgoing_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                         M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
    delete old_callback;

    outgoing_block_message_callback *new_callback = new outgoing_block_message_callback(callback);
    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
}
#endif

bool M2MResourceBase::set_notification_sent_callback(notification_sent_callback callback)
{
    notification_sent_callback *old_callback = (notification_sent_callback*)M2MCallbackStorage::remove_callback(*this,
                                                         M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
    delete old_callback;

    notification_sent_callback *new_callback = new notification_sent_callback(callback);
    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
}

bool M2MResourceBase::set_notification_sent_callback(notification_sent_callback_2 callback)
{
    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);

    return M2MCallbackStorage::add_callback(*this, (void*)callback, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);
}

void M2MResourceBase::notification_sent()
{
    // Now we will call both callbacks, if they are set. This is different from original behavior.
    notification_sent_callback* callback = (notification_sent_callback*)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
    if (callback) {
        (*callback)();
    }

    notification_sent_callback_2 callback2 = (notification_sent_callback_2)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);
    if (callback2) {
        (*callback2)();
    }
}



M2MResourceBase::ResourceType M2MResourceBase::convert_data_type(M2MBase::DataType type) const
{
    M2MResourceBase::ResourceType res_type = M2MResourceBase::OBJLINK;
    switch(type) {
        case M2MBase::STRING:
            res_type = M2MResourceBase::STRING;
            break;
        case M2MBase::INTEGER:
            res_type = M2MResourceBase::INTEGER;
            break;
        case M2MBase::FLOAT:
            res_type = M2MResourceBase::FLOAT;
            break;
        case M2MBase::OPAQUE:
            res_type = M2MResourceBase::OPAQUE;
            break;
        case M2MBase::BOOLEAN:
            res_type = M2MResourceBase::BOOLEAN;
            break;
        case M2MBase::TIME:
            res_type = M2MResourceBase::TIME;
            break;
        case M2MBase::OBJLINK:
            res_type = M2MResourceBase::OBJLINK;
            break;
    }
    return res_type;
}

