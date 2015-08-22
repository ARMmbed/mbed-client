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
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mconstants.h"
#include "include/m2mtlvserializer.h"
#include "include/m2mtlvdeserializer.h"
#include "include/nsdllinker.h"
#include "ns_trace.h"

M2MObject::M2MObject(const String &object_name)
: M2MBase(object_name,M2MBase::Dynamic)
{
    M2MBase::set_base_type(M2MBase::Object);
    if(M2MBase::name_id() != -1) {
        M2MBase::set_coap_content_type(99);
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
: M2MBase(other)
{
    this->operator=(other);
}

M2MObjectInstance* M2MObject::create_object_instance(uint16_t instance_id)
{
    tr_debug("M2MObject::create_object_instance()");
    M2MObjectInstance *instance = NULL;
    if(!object_instance(instance_id)) {
        instance = new M2MObjectInstance(this->name());
        instance->set_instance_id(instance_id);
        _instance_list.push_back(instance);
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

                    remove_resource_from_coap(obj_name);
                }

                delete obj;
                obj = NULL;
                _instance_list.erase(pos);
                success = true;
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

bool M2MObject::handle_observation_attribute(char *&query)
{
    tr_debug("M2MObject::handle_observation_attribute(query %s)", query);
    bool success = false;
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        for ( ; it != _instance_list.end(); it++ ) {
            success = (*it)->handle_observation_attribute(query);
        }
    }
    return success;
}

sn_coap_hdr_s* M2MObject::handle_get_request(nsdl_s *nsdl,
                                             sn_coap_hdr_s *received_coap_header,
                                             M2MObservationHandler *observation_handler)
{
    tr_debug("M2MObject::handle_get_request()");
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: GET for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    uint8_t * data = NULL;
    uint32_t  data_length = 0;
    //TODO: GET for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    if(received_coap_header) {
        // process the GET if we have registered a callback for it
        if ((operation() & SN_GRS_GET_ALLOWED) != 0) {
            coap_response = sn_nsdl_build_response(nsdl,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_CONTENT);
            if(coap_response) {
                if(received_coap_header->content_type_ptr){
                    coap_response->content_type_ptr = (uint8_t*)malloc(received_coap_header->content_type_len);
                    if(coap_response->content_type_ptr) {
                        memset(coap_response->content_type_ptr, 0, received_coap_header->content_type_len);
                        memcpy(coap_response->content_type_ptr,
                               received_coap_header->content_type_ptr,
                               received_coap_header->content_type_len);
                        coap_response->content_type_len = received_coap_header->content_type_len;
                     }
                } else {
                    uint8_t content_type = M2MBase::coap_content_type();

                    coap_response->content_type_ptr = (uint8_t*)malloc(1);
                    if(coap_response->content_type_ptr) {
                        memset(coap_response->content_type_ptr, 0, 1);
                        memcpy(coap_response->content_type_ptr,&content_type,1);
                        coap_response->content_type_len = 1;
                    }
                }
                // fill in the CoAP response payload
                if(*coap_response->content_type_ptr == COAP_CONTENT_OMA_TLV_TYPE) {
                    M2MTLVSerializer *serializer = new M2MTLVSerializer();
                    data = serializer->serialize(_instance_list, data_length);
                    delete serializer;
                } else if(*coap_response->content_type_ptr == COAP_CONTENT_OMA_JSON_TYPE) {
                    // TOD0: Implement JSON Format.
                    coap_response->msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT; // Content format not supported
                } else {
                    coap_response->msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT; // Content format not supported
                }

                coap_response->payload_len = data_length;
                coap_response->payload_ptr = data;

                coap_response->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
                memset(coap_response->options_list_ptr, 0, sizeof(sn_coap_options_list_s));


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
                            if(received_coap_header->options_list_ptr->observe_len == 0) {
                                set_under_observation(true,observation_handler);
                            }
                            else {
                            for(int i=0;i < received_coap_header->options_list_ptr->observe_len; i++) {
                                number = (*(received_coap_header->options_list_ptr->observe_ptr + i) & 0xff) <<
                                         8*(received_coap_header->options_list_ptr->observe_len- 1 - i);
                                }
                            }
                            // If the observe value is 0 means register for observation.
                            if(number == 0) {
                                tr_debug("M2MResource::handle_get_request - Put Resource under Observation");
                                set_under_observation(true,observation_handler);
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
                        }
                    }

                }
            }
        }else {
            tr_error("M2MResource::handle_get_request - Return COAP_MSG_CODE_RESPONSE_BAD_REQUEST");
            // Operation is not allowed.
            coap_response = sn_nsdl_build_response(nsdl,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_BAD_REQUEST);
            if(coap_response) {
                coap_response->options_list_ptr = 0;
                coap_response->content_type_ptr = 0;
            }
        }
    }
    return coap_response;
}

sn_coap_hdr_s* M2MObject::handle_put_request(nsdl_s *nsdl,
                                             sn_coap_hdr_s *received_coap_header,
                                             M2MObservationHandler *observation_handler)
{
    tr_debug("M2MObject::handle_put_request()");
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: PUT for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    if(received_coap_header) {
        if ((operation() & SN_GRS_PUT_ALLOWED) != 0) {
            sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
            if(received_coap_header->payload_ptr) {
                tr_debug("M2MObject::handle_put_request() - Update Object with new values");
                if(*received_coap_header->content_type_ptr == COAP_CONTENT_OMA_TLV_TYPE) {
                    M2MTLVDeserializer *deserializer = new M2MTLVDeserializer();
                    if(deserializer) {
                        if(deserializer->is_object_instance(received_coap_header->payload_ptr)) {
                            deserializer->deserialise_object_instances(received_coap_header->payload_ptr,
                                                                       received_coap_header->payload_len,
                                                                       _instance_list);
                        } else {
                            msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                        }
                        delete deserializer;
                    }
                } else {
                        msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                    }
                if(observation_handler) {
                    observation_handler->value_updated(this);
                }
            }
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
            coap_response = sn_nsdl_build_response(nsdl,
                                                   received_coap_header,
                                                   msg_code);
        } else {
            // Operation is not allowed.
            tr_error("M2MObject::handle_put_request() - COAP_MSG_CODE_RESPONSE_BAD_REQUEST");
            coap_response = sn_nsdl_build_response(nsdl,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_BAD_REQUEST);
            if(coap_response) {
                coap_response->options_list_ptr = 0;
                coap_response->content_type_ptr = 0;
            }
        }
    }
    return coap_response;
}

sn_coap_hdr_s* M2MObject::handle_post_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler)
{
    tr_debug("M2MObject::handle_post_request()");
    sn_coap_hdr_s * coap_response = NULL;
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    // process the POST if we have registered a callback for it
    if(received_coap_header) {
        if ((operation() & SN_GRS_POST_ALLOWED) != 0) {
            // Create object instance.
            create_object_instance();
            if(received_coap_header->payload_ptr) {
                tr_debug("M2MObject::handle_post_request() - Update Object with new values");
                if(*received_coap_header->content_type_ptr == COAP_CONTENT_OMA_TLV_TYPE) {
                    M2MTLVDeserializer *deserializer = new M2MTLVDeserializer();
                    if(deserializer) {
                        if(deserializer->is_object_instance(received_coap_header->payload_ptr)) {
                            deserializer->deserialise_object_instances(received_coap_header->payload_ptr,
                                                                       received_coap_header->payload_len,
                                                                       _instance_list);
                        } else {
                            msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                        }
                        delete deserializer;
                    }
                } else {
                        msg_code =COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
                    }
                if(received_coap_header->options_list_ptr &&
                   received_coap_header->options_list_ptr->uri_query_ptr) {
                    char *query = (char*)malloc(received_coap_header->options_list_ptr->uri_query_len+1);
                    if (query){
                        memset(query, 0, received_coap_header->options_list_ptr->uri_query_len+1);
                        memcpy(query,
                            received_coap_header->options_list_ptr->uri_query_ptr,
                            received_coap_header->options_list_ptr->uri_query_len);
                        memset(query + received_coap_header->options_list_ptr->uri_query_len,'\0',1);//String terminator
                       tr_debug("M2MObject::handle_post_request() - Query %s", query);
                        // if anything was updated, re-initialize the stored notification attributes
                        if (!handle_observation_attribute(query)){
                            tr_debug("M2MObject::handle_post_request() - Invalid query");
                            msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                        }
                        free(query);
                    }
                }
                if(observation_handler) {
                    observation_handler->value_updated(this);
                }
            }
        } else { // if ((object->operation() & SN_GRS_POST_ALLOWED) != 0)
            tr_error("M2MObject::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
        }
    } else { //if(received_coap_header)
        tr_error("M2MObject::handle_post_request - COAP_MSG_CODE_RESPONSE_NOT_FOUND");
        msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND; // 4.01
    }
    coap_response = sn_nsdl_build_response(nsdl,
                                           received_coap_header,
                                           msg_code);
    return coap_response;
}
