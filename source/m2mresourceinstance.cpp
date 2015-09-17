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
#include "mbed-client/m2mobjectinstance.h"
#include "include/m2mreporthandler.h"
#include "include/nsdllinker.h"
#include "ns_trace.h"

M2MResourceInstance& M2MResourceInstance::operator=(const M2MResourceInstance& other)
{
    if (this != &other) { // protect against invalid self-assignment

        if(_value) {
            free(_value);
            _value = NULL;
            _value_length = 0;
        }
        _value_length = other._value_length;
        if(other._value) {
            _value = (uint8_t *)malloc(other._value_length+1);
            if(_value) {
                memset(_value, 0, other._value_length+1);
                memcpy((uint8_t *)_value, (uint8_t *)other._value, other._value_length);
            }
        }
    }
    return *this;
}

M2MResourceInstance::M2MResourceInstance(const M2MResourceInstance& other)
: M2MBase(other),  
  _execute_callback(NULL),
  _value(NULL),
  _value_length(0),
  _resource_type(M2MResourceInstance::STRING),
  _object_instance_callback(other._object_instance_callback)
{
    this->operator=(other);
}

M2MResourceInstance::M2MResourceInstance(const String &res_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         M2MObjectInstanceCallback &object_instance_callback)
: M2MBase(res_name,
          M2MBase::Dynamic),
 _object_instance_callback(object_instance_callback),
 _execute_callback(NULL),
 _value(NULL),
 _value_length(0),
 _resource_type(type)
{
    M2MBase::set_resource_type(resource_type);
    M2MBase::set_base_type(M2MBase::Resource);
}

M2MResourceInstance::M2MResourceInstance(const String &res_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         M2MObjectInstanceCallback &object_instance_callback)
: M2MBase(res_name,
          M2MBase::Static),
 _object_instance_callback(object_instance_callback),
 _execute_callback(NULL),
 _value(NULL),
 _value_length(0),
 _resource_type(type)
{
    M2MBase::set_resource_type(resource_type);
    M2MBase::set_base_type(M2MBase::Resource);
    if( value != NULL && value_length > 0 ) {
        _value = (uint8_t *)malloc(value_length+1);
        if(_value) {
            memset(_value, 0, value_length+1);
            memcpy((uint8_t *)_value, (uint8_t *)value, value_length);
            _value_length = value_length;
        }
    }
}

M2MResourceInstance::~M2MResourceInstance()
{
    if(_value) {
        free(_value);
        _value = NULL;
        _value_length = 0;
    }
}

M2MBase::BaseType M2MResourceInstance::base_type() const
{
    return M2MBase::base_type();
}

M2MResourceInstance::ResourceType M2MResourceInstance::resource_instance_type() const
{
    return _resource_type;
}

bool M2MResourceInstance::handle_observation_attribute(char *&query)
{
    tr_debug("M2MResourceInstance::handle_observation_attribute()");
    if (_resource_type == M2MResourceInstance::INTEGER ||
        _resource_type == M2MResourceInstance::FLOAT ){
        return M2MBase::handle_observation_attribute(query);
    }
    else {
        tr_debug("M2MResourceInstance::handle_observation_attribute() - write attribute is not numerical");
        return false;
    }
}

void M2MResourceInstance::set_execute_function(execute_callback callback)
{
    _execute_callback = callback;
}

bool M2MResourceInstance::set_value(const uint8_t *value,
                                    const uint32_t value_length)
{
    bool success = false;
    if( value != NULL && value_length > 0 ) {
        success = true;
        if(_value) {
             free(_value);
             _value = NULL;
             _value_length = 0;
        }
        _value = (uint8_t *)malloc(value_length+1);
        if(_value) {
            memset(_value, 0, value_length+1);
            memcpy((uint8_t *)_value, (uint8_t *)value, value_length);
            _value_length = value_length;
            if(M2MBase::Dynamic == mode()) {
                M2MReportHandler *report_handler = M2MBase::report_handler();
                if( report_handler && _resource_type != M2MResourceInstance::STRING) {
                    report_handler->set_value(atof((const char*)_value));
                    M2MBase::Observation  observation_level = M2MBase::observation_level();
                    if(M2MBase::O_Attribute == observation_level ||
                       M2MBase::OI_Attribute == observation_level||
                       M2MBase::OOI_Attribute == observation_level) {
                        _object_instance_callback.notification_update(observation_level);
                    }
                }
            } else if(M2MBase::Static == mode()) {
                M2MObservationHandler *observation_handler = M2MBase::observation_handler();
                if(observation_handler) {
                    observation_handler->value_updated(this);
                }
            }
        }
    }
    return success;
}

void M2MResourceInstance::execute(void *arguments)
{
    tr_debug("M2MResourceInstance::execute");
    if(_execute_callback) {
        _execute_callback(arguments);
    }
}

void M2MResourceInstance::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    if(_value && _value_length > 0) {
        value = (uint8_t *)malloc(_value_length+1);
        if(value) {
            value_length = _value_length;
            memset(value, 0, _value_length+1);
            memcpy((uint8_t *)value, (uint8_t *)_value, value_length);
        }
    }
}

uint8_t* M2MResourceInstance::value() const
{
    return _value;
}

uint32_t M2MResourceInstance::value_length() const
{
    return _value_length;
}

sn_coap_hdr_s* M2MResourceInstance::handle_get_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler)
{
    tr_debug("M2MResourceInstance::handle_get_request()");
    sn_coap_hdr_s * coap_response = NULL;
    if(received_coap_header) {
        // process the GET if we have registered a callback for it
        if ((operation() & SN_GRS_GET_ALLOWED) != 0) {
            coap_response = sn_nsdl_build_response(nsdl,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_CONTENT);
            if(coap_response) {
                char *content_type = (char*)malloc(20);
                int content_type_size = snprintf(content_type, 20,"%x",coap_content_type());

                coap_response->content_type_ptr = (uint8_t*)malloc(content_type_size);
                if(coap_response->content_type_ptr) {
                    memset(coap_response->content_type_ptr, 0, content_type_size);
                    memcpy(coap_response->content_type_ptr,content_type,content_type_size);
                    coap_response->content_type_len = (uint8_t)content_type_size;
                }

                free(content_type);

                // fill in the CoAP response payload
                coap_response->payload_len = value_length();
                coap_response->payload_ptr = value();

                coap_response->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
                memset(coap_response->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

                coap_response->options_list_ptr->max_age_ptr = (uint8_t*)malloc(1);
                memset(coap_response->options_list_ptr->max_age_ptr,0,1);
                coap_response->options_list_ptr->max_age_len = 1;

                if(received_coap_header->token_ptr) {
                    tr_debug("M2MResourceInstance::handle_get_request - Sets Observation Token to resource");
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
                            tr_debug("M2MResourceInstance::handle_get_request - Starts Observation");
                            // If the observe length is 0 means register for observation.
                            if(received_coap_header->options_list_ptr->observe_len != 0) {
                                for(int i=0;i < received_coap_header->options_list_ptr->observe_len; i++) {
                                    number = (*(received_coap_header->options_list_ptr->observe_ptr + i) & 0xff) <<
                                             8*(received_coap_header->options_list_ptr->observe_len- 1 - i);
                                    }
                            }
                            // If the observe value is 0 means register for observation.
                            if(number == 0) {
                                tr_debug("M2MResourceInstance::handle_get_request - Put Resource under Observation");
                                set_under_observation(true,observation_handler);
                                M2MBase::add_observation_level(M2MBase::R_Attribute);
                                uint8_t *obs_number = (uint8_t*)malloc(3);
                                memset(obs_number,0,3);
                                uint8_t observation_number_length = 1;

                                uint16_t number = observation_number();

                                tr_debug("M2MResourceInstance::handle_get_request - Observation Number %d", number);
                                obs_number[0] = ((number>>8) & 0xFF);
                                obs_number[1] = (number & 0xFF);

                                if(number > 0xFF) {
                                    observation_number_length = 2;
                                }
                                coap_response->options_list_ptr->observe_ptr = obs_number;
                                coap_response->options_list_ptr->observe_len = observation_number_length;
                            }
                        } else if (STOP_OBSERVATION == observe_option) {
                            tr_debug("M2MResourceInstance::handle_get_request - Stops Observation");
                            set_under_observation(false,NULL);
                            M2MBase::remove_observation_level(M2MBase::R_Attribute);
                        }
                    }

                }
            }
        }else {
            tr_error("M2MResourceInstance::handle_get_request - Return COAP_MSG_CODE_RESPONSE_BAD_REQUEST");
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

sn_coap_hdr_s* M2MResourceInstance::handle_put_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler)
{
    tr_debug("M2MResourceInstance::handle_put_request()");
    sn_coap_hdr_s * coap_response = NULL;
    // process the PUT if we have registered a callback for it
    if(received_coap_header) {
        if ((operation() & SN_GRS_PUT_ALLOWED) != 0) {
            sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
            set_value(received_coap_header->payload_ptr, received_coap_header->payload_len);
            if(received_coap_header->payload_ptr) {
               tr_debug("M2MResourceInstance::handle_put_request() - Update Resource with new values");
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
                    tr_debug("M2MResourceInstance::handle_put_request() - Query %s", query);
                    // if anything was updated, re-initialize the stored notification attributes
                    if (!handle_observation_attribute(query)){
                        tr_debug("M2MResourceInstance::handle_put_request() - Invalid query");
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
            tr_error("M2MResourceInstance::handle_put_request() - COAP_MSG_CODE_RESPONSE_BAD_REQUEST");
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

sn_coap_hdr_s* M2MResourceInstance::handle_post_request(nsdl_s *nsdl,
                                                sn_coap_hdr_s *received_coap_header,
                                                M2MObservationHandler *observation_handler)
{
    tr_debug("M2MResourceInstance::handle_post_request()");
    sn_coap_hdr_s * coap_response = NULL;
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    // process the POST if we have registered a callback for it
    if(received_coap_header) {
        if ((operation() & SN_GRS_POST_ALLOWED) != 0) {
            void *arguments = NULL;
            if(received_coap_header->options_list_ptr) {
                if(received_coap_header->options_list_ptr->uri_query_ptr) {
                    arguments = (void*)malloc(received_coap_header->options_list_ptr->uri_query_len+1);
                    if (arguments){
                        memset(arguments, 0, received_coap_header->options_list_ptr->uri_query_len+1);
                        memcpy(arguments,
                            received_coap_header->options_list_ptr->uri_query_ptr,
                            received_coap_header->options_list_ptr->uri_query_len);
                    }
                }
            }
            tr_debug("M2MResourceInstance::handle_post_request - Execute resource function");
            execute(arguments);
            free(arguments);
        } else { // if ((object->operation() & SN_GRS_POST_ALLOWED) != 0)
            tr_error("M2MResourceInstance::handle_post_request - COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED");
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
        }
    } else { //if(object && received_coap_header)
        tr_error("M2MResourceInstance::handle_post_request - COAP_MSG_CODE_RESPONSE_NOT_FOUND");
        msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND; // 4.01
    }
    coap_response = sn_nsdl_build_response(nsdl,
                                           received_coap_header,
                                           msg_code);
    return coap_response;
}
