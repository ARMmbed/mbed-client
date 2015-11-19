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
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mobservationhandler.h"
#include "include/m2mreporthandler.h"
#include "include/m2mtlvserializer.h"
#include "include/m2mtlvdeserializer.h"
#include "include/nsdllinker.h"
#include "ns_trace.h"

M2MResource& M2MResource::operator=(const M2MResource& other)
{
    if (this != &other) { // protect against invalid self-assignment
        _has_multiple_instances = other._has_multiple_instances;
        if(!other._resource_instance_list.empty()){
            M2MResourceInstance* ins = NULL;
            M2MResourceInstanceList::const_iterator it;
            it = other._resource_instance_list.begin();
            for (; it!=other._resource_instance_list.end(); it++ ) {
                ins = *it;
                _resource_instance_list.push_back(new M2MResourceInstance(*ins));
            }
        }
    }
    return *this;
}

M2MResource::M2MResource(const M2MResource& other)
: M2MResourceInstance(other)
{
    this->operator=(other);
}

M2MResource::M2MResource(M2MObjectInstanceCallback &object_instance_callback,
                         const String &resource_name,
                         const String &resource_type,
                         M2MResourceInstance::ResourceType type,
                         const uint8_t *value,
                         const uint8_t value_length,
                         bool multiple_instance)
: M2MResourceInstance(resource_name, resource_type, type, value, value_length,
                      object_instance_callback),
  _has_multiple_instances(multiple_instance)
{
    M2MBase::set_base_type(M2MBase::Resource);
    M2MBase::set_operation(M2MBase::GET_ALLOWED);
    M2MBase::set_observable(false);
}

M2MResource::M2MResource(M2MObjectInstanceCallback &object_instance_callback,
                         const String &resource_name,
                         const String &resource_type,
                         M2MResourceInstance::ResourceType type,
                         bool observable,
                         bool multiple_instance)
: M2MResourceInstance(resource_name, resource_type, type,
                      object_instance_callback),
  _has_multiple_instances(multiple_instance)
{
    M2MBase::set_base_type(M2MBase::Resource);
    M2MBase::set_operation(M2MBase::GET_PUT_ALLOWED);
    M2MBase::set_observable(observable);
}

M2MResource::~M2MResource()
{
    if(!_resource_instance_list.empty()) {
        M2MResourceInstance* res = NULL;
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for (; it!=_resource_instance_list.end(); it++ ) {
            //Free allocated memory for resources.
            res = *it;
            delete res;
            res = NULL;
        }
        _resource_instance_list.clear();
    }
}

bool M2MResource::supports_multiple_instances() const
{
    return _has_multiple_instances;
}

bool M2MResource::remove_resource_instance(uint16_t inst_id)
{
    tr_debug("M2MResource::remove_resource(inst_id %d)", inst_id);
    bool success = false;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstance* res = NULL;
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        int pos = 0;
        for ( ; it != _resource_instance_list.end(); it++, pos++ ) {
            if(((*it)->instance_id() == inst_id)) {
                // Resource found and deleted.
                res = *it;
                delete res;
                res = NULL;
                _resource_instance_list.erase(pos);
                success = true;
                break;
            }
        }
    }
    return success;
}

M2MResourceInstance* M2MResource::resource_instance(uint16_t inst_id) const
{
    tr_debug("M2MResource::resource(resource_name inst_id %d)", inst_id);
    M2MResourceInstance *res = NULL;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for ( ; it != _resource_instance_list.end(); it++ ) {
            if(((*it)->instance_id() == inst_id)) {
                // Resource found.
                res = *it;
                break;
            }
        }
    }
    return res;
}

const M2MResourceInstanceList& M2MResource::resource_instances() const
{
    return _resource_instance_list;
}

uint16_t M2MResource::resource_instance_count() const
{
    return (uint16_t)_resource_instance_list.size();
}

bool M2MResource::handle_observation_attribute(char *&query)
{
    tr_debug("M2MResource::handle_observation_attribute()");
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for ( ; it != _resource_instance_list.end(); it++ ) {
            M2MReportHandler *report_handler = (*it)->report_handler();
            if(report_handler) {
                report_handler->set_notification_trigger();
            }
        }
    }
    return M2MBase::handle_observation_attribute(query);
}

void M2MResource::add_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::add_observation_level(observation_level);
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator inst;
        inst = _resource_instance_list.begin();
        for ( ; inst != _resource_instance_list.end(); inst++ ) {
            (*inst)->add_observation_level(observation_level);
        }
    }
}

void M2MResource::remove_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::remove_observation_level(observation_level);
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator inst;
        inst = _resource_instance_list.begin();
        for ( ; inst != _resource_instance_list.end(); inst++ ) {
            (*inst)->remove_observation_level(observation_level);
        }
    }
}

void M2MResource::add_resource_instance(M2MResourceInstance *res)
{
    tr_debug("M2MResource::add_resource_instance()");
    if(res) {
        _resource_instance_list.push_back(res);
    }
}

sn_coap_hdr_s* M2MResource::handle_get_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler)
{
    tr_debug("M2MResource::handle_get_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CONTENT;
    sn_coap_hdr_s * coap_response = NULL;
    if(_has_multiple_instances) {
        coap_response = sn_nsdl_build_response(nsdl,
                                               received_coap_header,
                                               msg_code);
        if(received_coap_header) {
            // process the GET if we have registered a callback for it
            if ((operation() & SN_GRS_GET_ALLOWED) != 0) {
                if(coap_response) {
                    uint16_t coap_content_type = 0;
                    bool content_type_present = false;
                    if(received_coap_header->content_type_ptr){
                        content_type_present = true;
                        coap_response->content_type_ptr = (uint8_t*)malloc(received_coap_header->content_type_len);
                        if(coap_response->content_type_ptr) {
                            memset(coap_response->content_type_ptr, 0, received_coap_header->content_type_len);
                            memcpy(coap_response->content_type_ptr,
                                   received_coap_header->content_type_ptr,
                                   received_coap_header->content_type_len);
                            coap_response->content_type_len = received_coap_header->content_type_len;
                            for(uint8_t i = 0; i < coap_response->content_type_len; i++) {
                                coap_content_type = (coap_content_type << 8) + (coap_response->content_type_ptr[i] & 0xFF);
                            }
                         }
                    }

                    if(!content_type_present &&
                       M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE) {
                        coap_content_type = COAP_CONTENT_OMA_TLV_TYPE;
                    }

                    tr_debug("M2MResource::handle_get_request() - Request Content-Type %d", coap_content_type);

                    uint8_t *data = NULL;
                    uint32_t data_length = 0;
                    // fill in the CoAP response payload
                    if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type) {
                        M2MTLVSerializer *serializer = new M2MTLVSerializer();
                        if(serializer) {
                            data = serializer->serialize(this, data_length);
                            delete serializer;
                        }
                    } else {
                        msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT; // Content format not supported
                    }

                    coap_response->payload_len = data_length;
                    coap_response->payload_ptr = data;

                    coap_response->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
                    memset(coap_response->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

                    coap_response->options_list_ptr->max_age_ptr = (uint8_t*)malloc(1);
                    memset(coap_response->options_list_ptr->max_age_ptr,0,1);
                    coap_response->options_list_ptr->max_age_len = 1;

                    if(received_coap_header->token_ptr) {
                        tr_debug("M2MResource::handle_get_request - Sets Observation Token to resource");
                        set_observation_token(received_coap_header->token_ptr,
                                              received_coap_header->token_len);
                    }

                    if(received_coap_header->options_list_ptr) {
                        if(received_coap_header->options_list_ptr->observe) {
                            if (is_observable()) {
                                uint32_t number = 0;
                                uint8_t observe_option = 0;
                                if(received_coap_header->options_list_ptr->observe_ptr) {
                                    observe_option = *received_coap_header->options_list_ptr->observe_ptr;
                                }
                                if(START_OBSERVATION == observe_option) {
                                    tr_debug("M2MResource::handle_get_request - Starts Observation");
                                    // If the observe length is 0 means register for observation.
                                    if(received_coap_header->options_list_ptr->observe_len != 0) {
                                        for(int i=0;i < received_coap_header->options_list_ptr->observe_len; i++) {
                                            number = (*(received_coap_header->options_list_ptr->observe_ptr + i) & 0xff) <<
                                                     8*(received_coap_header->options_list_ptr->observe_len- 1 - i);
                                            }
                                    }
                                    // If the observe value is 0 means register for observation.
                                    if(number == 0) {
                                        tr_debug("M2MResource::handle_get_request - Put Resource under Observation");
                                        M2MResourceInstanceList::const_iterator it;
                                        it = _resource_instance_list.begin();
                                        for (; it!=_resource_instance_list.end(); it++ ) {
                                            tr_debug("M2MResource::handle_get_request - set_resource_observer");
                                            (*it)->set_resource_observer(this);
                                        }

                                        set_under_observation(true,observation_handler);
                                        M2MBase::add_observation_level(M2MBase::R_Attribute);

                                        uint8_t *obs_number = (uint8_t*)malloc(3);
                                        memset(obs_number,0,3);
                                        uint8_t observation_number_length = 1;

                                        uint16_t number = observation_number();

                                        tr_debug("M2MResource::handle_get_request - Observation Number %d", number);
                                        obs_number[0] = ((number>>8) & 0xFF);
                                        obs_number[1] = (number & 0xFF);

                                        if(number > 0xFF) {
                                            observation_number_length = 2;
                                        }
                                        coap_response->options_list_ptr->observe_ptr = obs_number;
                                        coap_response->options_list_ptr->observe_len = observation_number_length;
                                    }
                                } else if (STOP_OBSERVATION == observe_option) {
                                    tr_debug("M2MResource::handle_get_request - Stops Observation");
                                    set_under_observation(false,NULL);
                                    M2MBase::remove_observation_level(M2MBase::R_Attribute);
                                    M2MResourceInstanceList::const_iterator it;
                                    it = _resource_instance_list.begin();
                                    for (; it!=_resource_instance_list.end(); it++ ) {
                                        (*it)->set_resource_observer(NULL);
                                    }
                                }
                                msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
                            }
                            else {
                                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
                            }
                        }
                    }
                }
            }else {
                tr_error("M2MResource::handle_get_request - Return COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
                // Operation is not allowed.
                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
            }
        }
        if(coap_response) {
            coap_response->msg_code = msg_code;
        }
    } else {
        coap_response = M2MResourceInstance::handle_get_request(nsdl,
                                                                received_coap_header,
                                                                observation_handler);
    }
    return coap_response;
}

sn_coap_hdr_s* M2MResource::handle_put_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler)
{
    tr_debug("M2MResource::handle_put_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    sn_coap_hdr_s * coap_response = NULL;
    if(_has_multiple_instances) {
        coap_response = sn_nsdl_build_response(nsdl,
                                               received_coap_header,
                                               msg_code);
        // process the PUT if we have registered a callback for it
        if(received_coap_header) {
            if ((operation() & SN_GRS_PUT_ALLOWED) != 0) {                
                uint16_t coap_content_type = 0;
                bool content_type_present = false;
                if(received_coap_header->content_type_ptr) {
                    if(coap_response) {
                        content_type_present = true;
                        coap_response->content_type_ptr = (uint8_t*)malloc(received_coap_header->content_type_len);
                        if(coap_response->content_type_ptr) {
                            memset(coap_response->content_type_ptr, 0, received_coap_header->content_type_len);
                            memcpy(coap_response->content_type_ptr,
                                   received_coap_header->content_type_ptr,
                                   received_coap_header->content_type_len);
                            coap_response->content_type_len = received_coap_header->content_type_len;
                            for(uint8_t i = 0; i < coap_response->content_type_len; i++) {
                                coap_content_type = (coap_content_type << 8) + (coap_response->content_type_ptr[i] & 0xFF);
                            }
                        }
                    }
                }
                if(!content_type_present &&
                   M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE) {
                    coap_content_type = COAP_CONTENT_OMA_TLV_TYPE;
                }

                tr_debug("M2MResource::handle_put_request() - Request Content-Type %d", coap_content_type);

                if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type) {
                    M2MTLVDeserializer *deserializer = new M2MTLVDeserializer();
                    if(deserializer) {
                        M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
                        error = deserializer->deserialize_resource_instances(received_coap_header->payload_ptr,
                                                                             received_coap_header->payload_len,
                                                                             *this,
                                                                             M2MTLVDeserializer::Put);
                        switch(error) {
                            case M2MTLVDeserializer::None:
                                if(observation_handler) {
                                    observation_handler->value_updated(this);
                                }
                                msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
                                break;
                            case M2MTLVDeserializer::NotFound:
                                msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
                                break;
                            case M2MTLVDeserializer::NotAllowed:
                                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
                                break;
                            case M2MTLVDeserializer::NotValid:
                                msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
                                break;
                        }
                        delete deserializer;
                    }
                } else {
                    msg_code =COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                } // if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type)

                if(received_coap_header->options_list_ptr &&
                   received_coap_header->options_list_ptr->uri_query_ptr) {
                    char *query = (char*)malloc(received_coap_header->options_list_ptr->uri_query_len+1);
                    if (query){
                        msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
                        memset(query, 0, received_coap_header->options_list_ptr->uri_query_len+1);
                        memcpy(query,
                            received_coap_header->options_list_ptr->uri_query_ptr,
                            received_coap_header->options_list_ptr->uri_query_len);
                        memset(query + received_coap_header->options_list_ptr->uri_query_len,'\0',1);//String terminator
                        tr_debug("M2MResource::handle_put_request() - Query %s", query);
                        // if anything was updated, re-initialize the stored notification attributes
                        if (!handle_observation_attribute(query)){
                            tr_debug("M2MResource::handle_put_request() - Invalid query");
                            msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                        }
                        free(query);
                    }
                }
            } else {
                // Operation is not allowed.
                tr_error("M2MResource::handle_put_request() - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
                msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
            }
        } else {
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
        }
        if(coap_response) {
            coap_response->msg_code = msg_code;
        }
    } else {
        coap_response = M2MResourceInstance::handle_put_request(nsdl,
                                                                received_coap_header,
                                                                observation_handler);
     }
    return coap_response;
}

sn_coap_hdr_s* M2MResource::handle_post_request(nsdl_s *nsdl,
                                                sn_coap_hdr_s *received_coap_header,
                                                M2MObservationHandler */*observation_handler*/)
{
    tr_debug("M2MResource::handle_post_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    sn_coap_hdr_s * coap_response = sn_nsdl_build_response(nsdl,
                                                           received_coap_header,
                                                           msg_code);
    // process the POST if we have registered a callback for it
    if(received_coap_header) {
        if ((operation() & SN_GRS_POST_ALLOWED) != 0) {
            void *arguments = NULL;
            if(received_coap_header->payload_ptr) {
                if(received_coap_header->payload_ptr) {
                    arguments = (void*)malloc(received_coap_header->payload_len+1);
                    if (arguments){
                        memset(arguments, 0, received_coap_header->payload_len+1);
                        memcpy(arguments,
                            received_coap_header->payload_ptr,
                            received_coap_header->payload_len);
                    }
                }
            }
            tr_debug("M2MResource::handle_post_request - Execute resource function");
            execute(arguments);
            free(arguments);
        } else { // if ((object->operation() & SN_GRS_POST_ALLOWED) != 0)
            tr_error("M2MResource::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
        }
    } else { //if(object && received_coap_header)
        tr_error("M2MResource::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
        msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.01
    }
    if(coap_response) {
        coap_response->msg_code = msg_code;
    }
    return coap_response;
}

void M2MResource::notification_update()
{
    tr_debug("M2MResource::notification_update()");
    M2MReportHandler *report_handler = M2MBase::report_handler();
    if(report_handler) {
        report_handler->set_notification_trigger();
    }
}
