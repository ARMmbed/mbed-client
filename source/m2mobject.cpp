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
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mconstants.h"
#include "include/m2mtlvserializer.h"
#include "include/m2mtlvdeserializer.h"
#include "include/nsdllinker.h"
#include "include/m2mreporthandler.h"
#include "ns_trace.h"

M2MObject::M2MObject(const String &object_name)
: M2MBase(object_name,M2MBase::Dynamic),
  _max_instance_count(MAX_UNINT_16_COUNT)
{
    M2MBase::set_base_type(M2MBase::Object);
    if(M2MBase::name_id() != -1) {
        M2MBase::set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
    }
}

M2MObject::~M2MObject()
{
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        M2MObjectInstance* obj = NULL;
        uint16_t index = 0;
        for (; it!=_instance_list.end(); it++, index++ ) {
            //Free allocated memory for object instances.
            obj = *it;

            char *obj_inst_id = (char*)malloc(20);
            if(obj_inst_id) {
                snprintf(obj_inst_id, 20,"%d",index);

                String obj_name = M2MBase::name();
                obj_name += String("/");
                obj_name += String(obj_inst_id);

                free(obj_inst_id);
                remove_resource_from_coap(obj_name);
            }

            delete obj;
            obj = NULL;
        }
        remove_object_from_coap();
        _instance_list.clear();
    }
}

M2MObject& M2MObject::operator=(const M2MObject& other)
{
    if (this != &other) { // protect against invalid self-assignment
        if(!other._instance_list.empty()){
            M2MObjectInstance* ins = NULL;
            M2MObjectInstanceList::const_iterator it;
            it = other._instance_list.begin();
            for (; it!=other._instance_list.end(); it++ ) {
                ins = *it;
                _instance_list.push_back(new M2MObjectInstance(*ins));
            }
        }
    }
    return *this;
}

M2MObject::M2MObject(const M2MObject& other)
: M2MBase(other),
  _max_instance_count(MAX_UNINT_16_COUNT)
{
    this->operator=(other);
}

M2MObjectInstance* M2MObject::create_object_instance(uint16_t instance_id)
{
    tr_debug("M2MObject::create_object_instance()");
    M2MObjectInstance *instance = NULL;
    if(!object_instance(instance_id)) {
        instance = new M2MObjectInstance(this->name(),*this);
        if(instance) {
            instance->set_instance_id(instance_id);
            _instance_list.push_back(instance);
            if(M2MBase::name_id() != -1) {
                instance->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
            }
        }
    }
    return instance;
}

bool M2MObject::remove_object_instance(uint16_t inst_id)
{
    tr_debug("M2MObject::remove_object_instance(inst_id %d)", inst_id);
    bool success = false;
    if(!_instance_list.empty()) {
        M2MObjectInstance* obj = NULL;
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        int pos = 0;
        for ( ; it != _instance_list.end(); it++, pos++ ) {
            if((*it)->instance_id() == inst_id) {
                // Instance found and deleted.
                obj = *it;

                char *obj_inst_id = (char*)malloc(20);
                if(obj_inst_id) {
                    snprintf(obj_inst_id, 20,"%d",obj->instance_id());

                    String obj_name = name();
                    obj_name += String("/");
                    obj_name += String(obj_inst_id);

                    free(obj_inst_id);

                    delete obj;
                    obj = NULL;
                    _instance_list.erase(pos);
                    success = true;

                    remove_resource_from_coap(obj_name);
                }
                break;
            }
        }
    }
    return success;
}

M2MObjectInstance* M2MObject::object_instance(uint16_t inst_id) const
{
    tr_debug("M2MObject::object_instance(inst_id %d)", inst_id);
    M2MObjectInstance *obj = NULL;
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        for ( ; it != _instance_list.end(); it++ ) {
            if((*it)->instance_id() == inst_id) {
                // Instance found.
                obj = *it;
                break;
            }
        }
    }
    return obj;
}

const M2MObjectInstanceList& M2MObject::instances() const
{
    return _instance_list;
}

uint16_t M2MObject::instance_count() const
{
    return (uint16_t)_instance_list.size();
}

M2MBase::BaseType M2MObject::base_type() const
{
    return M2MBase::base_type();
}

void M2MObject::add_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::add_observation_level(observation_level);
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        for ( ; it != _instance_list.end(); it++ ) {
            (*it)->add_observation_level(observation_level);
        }
    }
}

void M2MObject::remove_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::remove_observation_level(observation_level);
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        for ( ; it != _instance_list.end(); it++ ) {
            (*it)->remove_observation_level(observation_level);
        }
    }
}

sn_coap_hdr_s* M2MObject::handle_get_request(nsdl_s *nsdl,
                                             sn_coap_hdr_s *received_coap_header,
                                             M2MObservationHandler *observation_handler)
{
    tr_debug("M2MObject::handle_get_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CONTENT;
    sn_coap_hdr_s * coap_response = sn_nsdl_build_response(nsdl,
                                                           received_coap_header,
                                                           msg_code);
    //TODO: GET for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    uint8_t * data = NULL;
    uint32_t  data_length = 0;
    //TODO: GET for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    if(received_coap_header) {
        // process the GET if we have registered a callback for it
        if ((operation() & SN_GRS_GET_ALLOWED) != 0) {
            if(coap_response) {
                uint16_t coap_content_type = 0;
                bool content_type_present = false;
                if(received_coap_header->content_type_ptr) {
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

                tr_debug("M2MObject::handle_get_request() - Request Content-Type %d", coap_content_type);

                // fill in the CoAP response payload
                if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type) {
                    M2MTLVSerializer *serializer = new M2MTLVSerializer();
                    data = serializer->serialize(_instance_list, data_length);
                    delete serializer;
                } else { // TOD0: Implement JSON Format.
                    msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT; // Content format not supported
                }

                coap_response->payload_len = data_length;
                coap_response->payload_ptr = data;

                coap_response->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
                memset(coap_response->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

                coap_response->options_list_ptr->max_age_ptr = (uint8_t*)malloc(1);
                memset(coap_response->options_list_ptr->max_age_ptr,0,1);
                coap_response->options_list_ptr->max_age_len = 1;

                if(data){
                    if(received_coap_header->token_ptr) {
                        tr_debug("M2MResource::handle_get_request - Sets Observation Token to resource");
                        set_observation_token(received_coap_header->token_ptr,
                                              received_coap_header->token_len);
                    }

                    if(received_coap_header->options_list_ptr) {
                        if(received_coap_header->options_list_ptr->observe) {
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
                                    set_under_observation(true,observation_handler);
                                    add_observation_level(M2MBase::O_Attribute);
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
                                // If the observe options_list_ptr->observe_ptr value is 1 means de-register from observation.
                                set_under_observation(false,NULL);
                                remove_observation_level(M2MBase::O_Attribute);
                            }
                        }
                    }
                } else {
                    msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT; // Content format not supported
                }
            }
        }else {
            tr_error("M2MResource::handle_get_request - Return COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
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

sn_coap_hdr_s* M2MObject::handle_put_request(nsdl_s *nsdl,
                                             sn_coap_hdr_s *received_coap_header,
                                             M2MObservationHandler */*observation_handler*/)
{
    tr_debug("M2MObject::handle_put_request()");
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    sn_coap_hdr_s *coap_response = sn_nsdl_build_response(nsdl,
                                                          received_coap_header,
                                                          msg_code);
    if(received_coap_header) {
        if ((operation() & SN_GRS_PUT_ALLOWED) != 0) {

            if(received_coap_header->options_list_ptr &&
               received_coap_header->options_list_ptr->uri_query_ptr) {
                char *query = (char*)malloc(received_coap_header->options_list_ptr->uri_query_len+1);
                if (query){
                    memset(query, 0, received_coap_header->options_list_ptr->uri_query_len+1);
                    memcpy(query,
                        received_coap_header->options_list_ptr->uri_query_ptr,
                        received_coap_header->options_list_ptr->uri_query_len);
                    memset(query + received_coap_header->options_list_ptr->uri_query_len,'\0',1);//String terminator
                   tr_debug("M2MObject::handle_put_request() - Query %s", query);
                    // if anything was updated, re-initialize the stored notification attributes
                    if (!handle_observation_attribute(query)){
                        tr_debug("M2MObject::handle_put_request() - Invalid query");
                        msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                    }
                    free(query);
                }
            }
        } else {
            // Operation is not allowed.
            tr_error("M2MObject::handle_put_request() - COAP_MSG_CODE_RESPONSE_BAD_REQUEST");
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

sn_coap_hdr_s* M2MObject::handle_post_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler)
{
    tr_debug("M2MObject::handle_post_request()");    
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    // process the POST if we have registered a callback for it    
    sn_coap_hdr_s *coap_response = sn_nsdl_build_response(nsdl,
                                                          received_coap_header,
                                                          msg_code);
    if(received_coap_header) {
        if ((operation() & SN_GRS_POST_ALLOWED) != 0) {
            if(received_coap_header->payload_ptr) {
                tr_debug("M2MObject::handle_post_request() - Update Object with new values");
                uint16_t coap_content_type = 0;
                bool content_type_present = false;
                if(received_coap_header->content_type_ptr) {
                    content_type_present = true;
                    if(coap_response) {
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
                } // if(received_coap_header->content_type_ptr)
                if(!content_type_present &&
                   M2MBase::coap_content_type() == COAP_CONTENT_OMA_TLV_TYPE) {
                    coap_content_type = COAP_CONTENT_OMA_TLV_TYPE;
                }

                tr_debug("M2MObject::handle_post_request() - Request Content-Type %d", coap_content_type);

                if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type) {
                    uint16_t instance_id = 0;
                    for(instance_id = 0; instance_id <= _max_instance_count; instance_id++) {
                        if(NULL == object_instance(instance_id)) {
                            break;
                        }
                        if(instance_id == _max_instance_count) {
                            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
                            break;
                        }
                    }
                    if(COAP_MSG_CODE_RESPONSE_CHANGED == msg_code) {
                        M2MObjectInstance *obj_instance = create_object_instance(instance_id);
                        if(obj_instance) {
                            obj_instance->set_operation(M2MBase::GET_PUT_ALLOWED);
                        }
                        M2MTLVDeserializer *deserializer = new M2MTLVDeserializer();
                        if(deserializer) {
                            M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
                            if(deserializer->is_object_instance(received_coap_header->payload_ptr)) {
                                tr_debug("M2MObject::handle_post_request() - TLV data contains ObjectInstance");
                                error = deserializer->deserialise_object_instances(received_coap_header->payload_ptr,
                                                                           received_coap_header->payload_len,
                                                                           *this,
                                                                           M2MTLVDeserializer::Post);
                            } else if(deserializer->is_resource(received_coap_header->payload_ptr) ||
                                      deserializer->is_multiple_resource(received_coap_header->payload_ptr)) {
                                tr_debug("M2MObject::handle_post_request() - TLV data contains Resources");
                                error = deserializer->deserialize_resources(received_coap_header->payload_ptr,
                                                                            received_coap_header->payload_len,
                                                                            *obj_instance,
                                                                            M2MTLVDeserializer::Post);
                            } else {
                                error = M2MTLVDeserializer::NotValid;
                            }
                            switch(error) {
                                case M2MTLVDeserializer::None:
                                    if(observation_handler) {
                                        observation_handler->value_updated(this);
                                    }
                                    msg_code = COAP_MSG_CODE_RESPONSE_CREATED;
                                    break;
                                case M2MTLVDeserializer::NotAllowed:
                                    msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
                                    break;
                                case M2MTLVDeserializer::NotValid:
                                    msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
                                    break;
                                case M2MTLVDeserializer::NotFound:
                                    msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
                                    break;
                            }
                            delete deserializer;
                        }
                    }
                } else {
                    msg_code =COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                } // if(COAP_CONTENT_OMA_TLV_TYPE == coap_content_type)
            } else {
                tr_error("M2MObject::handle_post_request - COAP_MSG_CODE_RESPONSE_BAD_REQUEST - Missing Payload");
                msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; //
            }
        } else { // if ((object->operation() & SN_GRS_POST_ALLOWED) != 0)
            tr_error("M2MObject::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
        }
    } else { //if(received_coap_header)
        tr_error("M2MObject::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
        msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
    }
    if(coap_response) {
        coap_response->msg_code = msg_code;
    }
    return coap_response;
}

void M2MObject::notification_update()
{
    M2MReportHandler *report_handler = M2MBase::report_handler();
    if(report_handler) {
        report_handler->trigger_object_notification();
    }
}
