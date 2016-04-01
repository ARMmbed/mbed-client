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
#include "mbed-client/m2mstring.h"
#include "include/nsdlaccesshelper.h"
#include "include/m2mnsdlobserver.h"
#include "mbed-client/m2msecurity.h"
#include "mbed-client/m2mserver.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconstants.h"
#include "include/m2mtlvserializer.h"
#include "ip6string.h"
#include "mbed-trace/mbed_trace.h"
#include "source/libNsdl/src/include/sn_grs.h"
#include "mbed-client/m2mtimer.h"

#include <assert.h>

#define BUFFER_SIZE 21

M2MNsdlInterface::M2MNsdlInterface(M2MNsdlObserver &observer)
: _observer(observer),
  _server(NULL),
  _nsdl_exceution_timer(new M2MTimer(*this)),
  _registration_timer(new M2MTimer(*this)),
  _nsdl_handle(NULL),
  _counter_for_nsdl(0),
  _register_id(0),
  _unregister_id(0),
  _update_id(0),
  _bootstrap_id(0)
{
    tr_debug("M2MNsdlInterface::M2MNsdlInterface()");
    _endpoint = NULL;
    _resource = NULL;
    __nsdl_interface = this;

    _bootstrap_endpoint.device_object = NULL;
    _bootstrap_endpoint.oma_bs_status_cb = NULL;

    _bootstrap_device_setup.sn_oma_device_boot_callback = NULL;
    _bootstrap_device_setup.error_code = NO_ERROR;

    _sn_nsdl_address.addr_len = 0;
    _sn_nsdl_address.addr_ptr = NULL;
    _sn_nsdl_address.port = 0;

    // This initializes libCoap and libNsdl
    // Parameters are function pointers to used memory allocation
    // and free functions in structure and used functions for sending
    // and receiving purposes.
    _nsdl_handle = sn_nsdl_init(&(__nsdl_c_send_to_server), &(__nsdl_c_received_from_server),
                 &(__nsdl_c_memory_alloc), &(__nsdl_c_memory_free));

    initialize();
}

M2MNsdlInterface::~M2MNsdlInterface()
{
    tr_debug("M2MNsdlInterface::~M2MNsdlInterface() - IN");
    if(_resource) {
        if(_resource->resource_parameters_ptr) {
            memory_free(_resource->resource_parameters_ptr);
            _resource->resource_parameters_ptr = NULL;
        }
        memory_free(_resource);
        _resource = NULL;
    }
    if(_endpoint) {
        if(_endpoint->lifetime_ptr) {
            memory_free(_endpoint->lifetime_ptr);
            _endpoint->lifetime_ptr = NULL;
        }
        if(_endpoint->location_ptr) {
            memory_free(_endpoint->location_ptr);
            _endpoint->location_ptr = NULL;
        }
        memory_free(_endpoint);
        _endpoint = NULL;

    }
    delete _nsdl_exceution_timer;
    delete _registration_timer;
    _object_list.clear();

    if(_server){
        delete _server;
        _server = NULL;
    }
    sn_nsdl_destroy(_nsdl_handle);
    _nsdl_handle = NULL;
    __nsdl_interface = NULL;
    tr_debug("M2MNsdlInterface::~M2MNsdlInterface() - OUT");
}

bool M2MNsdlInterface::initialize()
{
    tr_debug("M2MNsdlInterface::initialize()");
    bool success = false;

    //Sets the packet retransmission attempts and time interval
    sn_nsdl_set_retransmission_parameters(_nsdl_handle,RETRY_COUNT,RETRY_INTERVAL);

    _nsdl_exceution_timer->start_timer(ONE_SECOND_TIMER * 1000,
                                       M2MTimerObserver::NsdlExecution,
                                       false);

    // Allocate the memory for resources
    _resource = (sn_nsdl_resource_info_s*)memory_alloc(sizeof(sn_nsdl_resource_info_s));
    if(_resource) {
        memset(_resource, 0, sizeof(sn_nsdl_resource_info_s));
        _resource->resource_parameters_ptr = (sn_nsdl_resource_parameters_s*)memory_alloc(sizeof(sn_nsdl_resource_parameters_s)+1);
        if(_resource->resource_parameters_ptr) {
            memset(_resource->resource_parameters_ptr, 0, sizeof(sn_nsdl_resource_parameters_s)+1);
        }
    }

    //Allocate the memory for endpoint
    _endpoint = (sn_nsdl_ep_parameters_s*)memory_alloc(sizeof(sn_nsdl_ep_parameters_s)+1);
    if(_endpoint) {
        memset(_endpoint, 0, sizeof(sn_nsdl_ep_parameters_s)+1);
        success = true;
    }
    return success;
}

void M2MNsdlInterface::create_endpoint(const String &name,
                                       const String &type,
                                       const int32_t life_time,
                                       const String &domain,
                                       const uint8_t mode,
                                       const String &/*context_address*/)
{
    tr_debug("M2MNsdlInterface::create_endpoint( name %s type %s lifetime %d, domain %s, mode %d)",
              name.c_str(), type.c_str(), life_time, domain.c_str(), mode);
    if(_endpoint){
        memset(_endpoint, 0, sizeof(sn_nsdl_ep_parameters_s)+1);
        if(!name.empty()) {
            _endpoint->endpoint_name_ptr = (uint8_t*)name.c_str();
            _endpoint->endpoint_name_len = name.length();
        }
        if(!type.empty()) {
            _endpoint->type_ptr = (uint8_t*)type.c_str();
            _endpoint->type_len =  type.length();
        }
        if(!domain.empty()) {
            _endpoint->domain_name_ptr = (uint8_t*)domain.c_str();
            _endpoint->domain_name_len = domain.length();
        }
        _endpoint->binding_and_mode = (sn_nsdl_oma_binding_and_mode_t)mode;

        // If lifetime is less than zero then leave the field empty
        if( life_time > 0) {
            set_endpoint_lifetime_buffer(life_time);
        }
    }
}

void M2MNsdlInterface::set_endpoint_lifetime_buffer(int lifetime)
{
    // max len of "-9223372036854775808" plus zero termination
    char buffer[20+1];
    
    uint32_t size = m2m::itoa_c(lifetime, buffer);

    if (size <= sizeof(buffer)) {
        _endpoint->lifetime_ptr = alloc_string_copy((uint8_t*)buffer, size);
        if(_endpoint->lifetime_ptr) {
            _endpoint->lifetime_len =  size;
        } else {
            _endpoint->lifetime_len = 0;
        }
    }
}


void M2MNsdlInterface::delete_endpoint()
{
    tr_debug("M2MNsdlInterface::delete_endpoint()");
    if(_endpoint) {
        if(_endpoint->lifetime_ptr) {
            free(_endpoint->lifetime_ptr);
            _endpoint->lifetime_ptr = NULL;
        }
        memory_free(_endpoint);
        _endpoint = NULL;
    }
}

bool M2MNsdlInterface::create_nsdl_list_structure(const M2MObjectList &object_list)
{
    tr_debug("M2MNsdlInterface::create_nsdl_list_structure()");
    bool success = false;
    if(!object_list.empty()) {
       tr_debug("M2MNsdlInterface::create_nsdl_list_structure - Object count is %d", object_list.size());
        M2MObjectList::const_iterator it;
        it = object_list.begin();
        for ( ; it != object_list.end(); it++ ) {
            // Create NSDL structure for all Objects inside
            success = create_nsdl_object_structure(*it);
            add_object_to_list(*it);
        }
    }
    return success;
}

bool M2MNsdlInterface::delete_nsdl_resource(const String &resource_name)
{
    tr_debug("M2MNsdlInterface::delete_nsdl_resource( %s)", resource_name.c_str());
    return (sn_nsdl_delete_resource(_nsdl_handle,
                                    resource_name.length(),
                                    (uint8_t *)resource_name.c_str()) == 0) ? true : false;
}


bool M2MNsdlInterface::create_bootstrap_resource(sn_nsdl_addr_s *address)
{
#ifndef YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
    tr_debug("M2MNsdlInterface::create_bootstrap_resource()");
    bool success = false;
    _bootstrap_device_setup.error_code = NO_ERROR;
    _bootstrap_device_setup.sn_oma_device_boot_callback = 0;

    _bootstrap_endpoint.device_object = &_bootstrap_device_setup;
    _bootstrap_endpoint.oma_bs_status_cb = &__nsdl_c_bootstrap_done;

    if(_bootstrap_id == 0) {
        _bootstrap_id = sn_nsdl_oma_bootstrap(_nsdl_handle,
                                               address,
                                               _endpoint,
                                               &_bootstrap_endpoint);
        tr_debug("M2MNsdlInterface::create_bootstrap_resource - _bootstrap_id %d", _bootstrap_id);
        success = _bootstrap_id != 0;

    }
    return success;
#else
    return false;
#endif //YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
}

bool M2MNsdlInterface::send_register_message(uint8_t* address,
                                             const uint16_t port,
                                             sn_nsdl_addr_type_e address_type)
{
    tr_debug("M2MNsdlInterface::send_register_message()");
    bool success = false;
    if(set_NSP_address(_nsdl_handle,address, port, address_type) == 0) {
        if(_register_id == 0) {
            _register_id = sn_nsdl_register_endpoint(_nsdl_handle,_endpoint);
            tr_debug("M2MNsdlInterface::send_register_message - _register_id %d", _register_id);
            success = _register_id != 0;
        }
    }
    return success;
}

bool M2MNsdlInterface::send_update_registration(const uint32_t lifetime)
{
    tr_debug("M2MNsdlInterface::send_update_registration( lifetime %d)", lifetime);
    bool success = false;

    create_nsdl_list_structure(_object_list);
    //If Lifetime value is 0, then don't change the existing lifetime value
    if(lifetime != 0) {
        if(_endpoint->lifetime_ptr) {
            memory_free(_endpoint->lifetime_ptr);
            _endpoint->lifetime_ptr = NULL;
            _endpoint->lifetime_len = 0;
        }
        set_endpoint_lifetime_buffer(lifetime);
        
        _registration_timer->stop_timer();
        _registration_timer->start_timer(registration_time() * 1000,
                                         M2MTimerObserver::Registration,
                                         false);
        if(_nsdl_handle &&
           _endpoint && _endpoint->lifetime_ptr) {
            _update_id = sn_nsdl_update_registration(_nsdl_handle,
                                                     _endpoint->lifetime_ptr,
                                                     _endpoint->lifetime_len);
            tr_debug("M2MNsdlInterface::send_update_registration - New lifetime value _update_id %d", _update_id);
            success = _update_id != 0;
        }
    } else {
        if(_nsdl_handle) {
            _update_id = sn_nsdl_update_registration(_nsdl_handle, NULL, 0);
            tr_debug("M2MNsdlInterface::send_update_registration - regular update- _update_id %d", _update_id);
            success = _update_id != 0;
        }
    }
    return success;
}

bool M2MNsdlInterface::send_unregister_message()
{
    tr_debug("M2MNsdlInterface::send_unregister_message");
    bool success = false;
    //Does not clean resources automatically
    if(_unregister_id == 0) {
       _unregister_id = sn_nsdl_unregister_endpoint(_nsdl_handle);
       tr_debug("M2MNsdlInterface::send_unregister_message - _unregister_id %d", _unregister_id);
       success = _unregister_id != 0;
    }
    return success;
}

// XXX: move these to common place, no need to copy these wrappers to multiple places:
void *M2MNsdlInterface::memory_alloc(uint16_t size)
{
    if(size)
        return malloc(size);
    else
        return 0;
}

void M2MNsdlInterface::memory_free(void *ptr)
{
    if(ptr)
        free(ptr);
}

uint8_t* M2MNsdlInterface::alloc_string_copy(const uint8_t* source, uint16_t size)
{
    assert(source != NULL);

    uint8_t* result = (uint8_t*)memory_alloc(size + 1);
    if (result) {
        memcpy(result, source, size);
        result[size] = '\0';
    }
    return result;
}

uint8_t M2MNsdlInterface::send_to_server_callback(struct nsdl_s * /*nsdl_handle*/,
                                                  sn_nsdl_capab_e /*protocol*/,
                                                  uint8_t *data_ptr,
                                                  uint16_t data_len,
                                                  sn_nsdl_addr_s *address)
{
    tr_debug("M2MNsdlInterface::send_to_server_callback()");
    _observer.coap_message_ready(data_ptr,data_len,address);
    return 1;
}

uint8_t M2MNsdlInterface::received_from_server_callback(struct nsdl_s * /*nsdl_handle*/,
                                                        sn_coap_hdr_s *coap_header,
                                                        sn_nsdl_addr_s *address)
{
    tr_debug("M2MNsdlInterface::received_from_server_callback()");
    _observer.coap_data_processed();
    uint8_t value = 0;
    if(coap_header) {
        if(coap_header->msg_id == _register_id) {
            _register_id = 0;
            if(coap_header->msg_code == COAP_MSG_CODE_RESPONSE_CREATED) {
                if(_server) {
                    delete _server;
                    _server = NULL;
                }
                tr_debug("M2MNsdlInterface::received_from_server_callback - registration callback");
                _server = new M2MServer();
                _server->set_resource_value(M2MServer::ShortServerID,1);

                _observer.client_registered(_server);
                // If lifetime is less than zero then leave the field empty
                if(coap_header->options_list_ptr) {
                    if(coap_header->options_list_ptr->max_age_ptr) {
                        if(_endpoint->lifetime_ptr) {
                            memory_free(_endpoint->lifetime_ptr);
                            _endpoint->lifetime_ptr = NULL;
                            _endpoint->lifetime_len = 0;
                        }
                        uint32_t max_time = 0;
                        for(int i=0;i < coap_header->options_list_ptr->max_age_len; i++) {
                            max_time += (*(coap_header->options_list_ptr->max_age_ptr + i) & 0xff) <<
                                     8*(coap_header->options_list_ptr->max_age_len- 1 - i);
                            }
                        // If lifetime is less than zero then leave the field empty
                        if( max_time > 0) {
                            set_endpoint_lifetime_buffer(max_time);
                        }
                    }
                    if(coap_header->options_list_ptr->location_path_ptr) {
                        _endpoint->location_ptr = alloc_string_copy(coap_header->options_list_ptr->location_path_ptr, coap_header->options_list_ptr->location_path_len);
                        if (_endpoint->location_ptr != NULL) {
                            _endpoint->location_len = coap_header->options_list_ptr->location_path_len;
                        }
                        sn_nsdl_set_endpoint_location(_nsdl_handle,_endpoint->location_ptr,_endpoint->location_len);
                    }
                }
                if(_endpoint->lifetime_ptr) {
                    _registration_timer->stop_timer();
                    _registration_timer->start_timer(registration_time() * 1000,
                                                     M2MTimerObserver::Registration,
                                                     false);
                }
            } else {
                if(_server) {
                    delete _server;
                    _server = NULL;
                }
                tr_error("M2MNsdlInterface::received_from_server_callback - registration error %d", coap_header->msg_code);
                M2MInterface::Error error = interface_error(coap_header);
                _observer.registration_error(error);
            }
        } else if(coap_header->msg_id == _unregister_id) {
            _unregister_id = 0;
            tr_debug("M2MNsdlInterface::received_from_server_callback - unregistration callback");
            if(coap_header->msg_code == COAP_MSG_CODE_RESPONSE_DELETED) {
                _registration_timer->stop_timer();
                if(_server) {
                   delete _server;
                   _server = NULL;
                }
                _observer.client_unregistered();
            } else {
                tr_error("M2MNsdlInterface::received_from_server_callback - unregistration error %d", coap_header->msg_code);
                M2MInterface::Error error = interface_error(coap_header);
                _observer.registration_error(error);
            }
        } else if(coap_header->msg_id == _update_id) {
            _update_id = 0;

            if(coap_header->msg_code == COAP_MSG_CODE_RESPONSE_CHANGED) {
                tr_debug("M2MNsdlInterface::received_from_server_callback - registration_updated successfully");
                _observer.registration_updated(*_server);
            } else {
                tr_error("M2MNsdlInterface::received_from_server_callback - registration_updated failed %d", coap_header->msg_code);                
                _register_id = sn_nsdl_register_endpoint(_nsdl_handle,_endpoint);
            }
        }
#ifndef YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
        else if(coap_header->msg_id == _bootstrap_id) {
            _bootstrap_id = 0;
            M2MInterface::Error error = interface_error(coap_header);
            if(error != M2MInterface::ErrorNone) {
                _observer.bootstrap_error();
            }
        }
#endif //YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
        else {
            if(COAP_MSG_CODE_REQUEST_POST == coap_header->msg_code) {
                if(coap_header->uri_path_ptr) {
                    bool execute_value_updated = false;
                    M2MObjectInstance *obj_instance = NULL;
                    String resource_name = coap_to_string(coap_header->uri_path_ptr,
                                                          coap_header->uri_path_len);

                    sn_coap_hdr_s *coap_response = NULL;
                    String object_name;
                    int slash_found = resource_name.find_last_of('/');
                    //The POST operation here is only allowed for non-existing object instances
                    if(slash_found != -1) {
                        object_name = resource_name.substr(0,slash_found);
                        if( object_name.find_last_of('/') != -1){
                            coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                                   coap_header,
                                                                   COAP_MSG_CODE_RESPONSE_NOT_FOUND);
                        } else {
                            int32_t instance_id = atoi(resource_name.substr(slash_found+1,
                                                     resource_name.size()-object_name.size()).c_str());
                            M2MBase* base = find_resource(object_name);
                            if(base && (instance_id >= 0) && (instance_id < 65535)) {
                                if(coap_header->payload_ptr) {
                                    M2MObject* object = (M2MObject*)base;
                                    obj_instance = object->create_object_instance(instance_id);
                                    if(obj_instance) {
                                        obj_instance->set_operation(M2MBase::GET_PUT_POST_ALLOWED);                                        
                                        coap_response = obj_instance->handle_post_request(_nsdl_handle,
                                                                                          coap_header,
                                                                                          this,
                                                                                          execute_value_updated);
                                    }
                                    if(coap_response && coap_response->msg_code != COAP_MSG_CODE_RESPONSE_CREATED) {
                                        //Invalid request so remove created ObjectInstance
                                        object->remove_object_instance(instance_id);
                                    } else  {
                                        tr_debug("M2MNsdlInterface::received_from_server_callback - Send Update registration for Create");
                                        send_update_registration();
                                    }
                                } else {
                                    tr_debug("M2MNsdlInterface::received_from_server_callback - Missing Payload - Cannot create");
                                    coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                                           coap_header,
                                                                           COAP_MSG_CODE_RESPONSE_BAD_REQUEST);
                                }
                            } else { //if(base)
                                tr_debug("M2MNsdlInterface::received_from_server_callback - Missing BASE - Cannot create");
                                coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                                       coap_header,
                                                                       COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED);
                            }
                        }
                    } else{ // if(slash_found != -1)
                        tr_debug("M2MNsdlInterface::received_from_server_callback - slash_found - Cannot create");
                        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                               coap_header,
                                                               COAP_MSG_CODE_RESPONSE_NOT_FOUND);
                    }
                    if(coap_response) {
                        tr_debug("M2MNsdlInterface::received_from_server_callback - send CoAP response");
                        (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? value = 0 : value = 1;
                        sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
                    }
                    if (execute_value_updated) {
                        value_updated(obj_instance, obj_instance->name());
                    }
                }
            }
        }
    }
    return value;
}

uint8_t M2MNsdlInterface::resource_callback(struct nsdl_s */*nsdl_handle*/,
                                            sn_coap_hdr_s *received_coap_header,
                                            sn_nsdl_addr_s *address,
                                            sn_nsdl_capab_e /*nsdl_capab*/)
{
    tr_debug("M2MNsdlInterface::resource_callback()");
    _observer.coap_data_processed();
    uint8_t result = 1;
    sn_coap_hdr_s *coap_response = NULL;
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 4.00
    String resource_name = coap_to_string(received_coap_header->uri_path_ptr,
                                          received_coap_header->uri_path_len);
    tr_debug("M2MNsdlInterface::resource_callback() - resource_name %s", resource_name.c_str());
    bool execute_value_updated = false;
    M2MBase* base = find_resource(resource_name);
    if(base) {
        base->set_uri_path(resource_name);
        if(COAP_MSG_CODE_REQUEST_GET == received_coap_header->msg_code) {
            coap_response = base->handle_get_request(_nsdl_handle, received_coap_header,this);
        } else if(COAP_MSG_CODE_REQUEST_PUT == received_coap_header->msg_code) {            
            coap_response = base->handle_put_request(_nsdl_handle, received_coap_header, this, execute_value_updated);
        } else if(COAP_MSG_CODE_REQUEST_POST == received_coap_header->msg_code) {
            if(base->base_type() == M2MBase::ResourceInstance) {
                msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
            } else {
                coap_response = base->handle_post_request(_nsdl_handle, received_coap_header,this, execute_value_updated);
            }
        } else if(COAP_MSG_CODE_REQUEST_DELETE == received_coap_header->msg_code) {
            // Delete the object instance
            tr_debug("M2MNsdlInterface::resource_callback() - DELETE the object instance");
            M2MBase::BaseType type = base->base_type();
            if(M2MBase::ObjectInstance == type) {
                M2MBase* base_object = find_resource(base->name());
                if(base_object) {
                    M2MObject *object = (M2MObject*)base_object;
                    int slash_found = resource_name.find_last_of('/');
                    // Object instance validty checks done in upper level, no need for error handling
                    if(slash_found != -1) {
                        String object_name;
                        object_name = resource_name.substr(slash_found + 1, resource_name.length());
                        if (object->remove_object_instance(strtoul(
                                object_name.c_str(),
                                NULL,
                                10))) {
                            msg_code = COAP_MSG_CODE_RESPONSE_DELETED;
                        }
                    }
                }
            } else {
                msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
            }
        } else if(COAP_MSG_TYPE_RESET == received_coap_header->msg_type) {
            // Cancel ongoing observation
            tr_debug("M2MNsdlInterface::resource_callback() - RESET msg");
            M2MBase::BaseType type = base->base_type();
            switch (type) {
                case M2MBase::Object:
                    base->remove_observation_level(M2MBase::O_Attribute);
                    break;
                case M2MBase::Resource:
                    base->remove_observation_level(M2MBase::R_Attribute);
                    break;
                case M2MBase::ObjectInstance:
                    base->remove_observation_level(M2MBase::OI_Attribute);
                    break;
                default:
                    break;
            }
            base->set_under_observation(false, this);
        }
    } else  {
        tr_debug("M2MNsdlInterface::resource_callback() - Resource NOT FOUND");
        msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
    }
    if(!coap_response) {
            coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                   received_coap_header,
                                                   msg_code);
    }

    if(coap_response) {
        tr_debug("M2MNsdlInterface::resource_callback() - send CoAP response");
        (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
        if(coap_response->payload_ptr) {
            free(coap_response->payload_ptr);
            coap_response->payload_ptr = NULL;
        }
        sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
    }
    if (execute_value_updated) {
        value_updated(base,base->uri_path());
    }
    return result;
}


void M2MNsdlInterface::bootstrap_done_callback(sn_nsdl_oma_server_info_t *server_info)
{
#ifndef YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
    tr_debug("M2MNsdlInterface::bootstrap_done_callback()");
    _bootstrap_id = 0;
    M2MSecurity* security = NULL;
    if(server_info && server_info->omalw_address_ptr->addr_ptr) {
        security = new M2MSecurity(M2MSecurity::M2MServer);
        uint16_t port = server_info->omalw_address_ptr->port;

        String server_port;
        server_port.append_int(port);

        String server_uri(COAP);
        String server_address;
        //TODO: currently only supports IPV4 Mapping, fix to support IPV6 as well
        if(SN_NSDL_ADDRESS_TYPE_IPV4 == server_info->omalw_address_ptr->type) {
            int val = 0;
            for(int index = 0; index < server_info->omalw_address_ptr->addr_len; index++) {
                val = (int)server_info->omalw_address_ptr->addr_ptr[index];

                server_address.append_int(val);

                if(index < server_info->omalw_address_ptr->addr_len-1) {
                    server_address.push_back('.');
                }
            }

            tr_debug("M2MNsdlInterface::bootstrap_done_callback - IPv4 Server address received %s", server_address.c_str());
        } else if(SN_NSDL_ADDRESS_TYPE_HOSTNAME == server_info->omalw_address_ptr->type) {

            server_address.append_raw((char*)server_info->omalw_address_ptr->addr_ptr, server_info->omalw_address_ptr->addr_len);
            tr_debug("M2MNsdlInterface::bootstrap_done_callback - Hostname Server address received %s", server_address.c_str());

        } else if(SN_NSDL_ADDRESS_TYPE_IPV6 == server_info->omalw_address_ptr->type) {
            char ipv6_address[40];
            ip6tos(server_info->omalw_address_ptr->addr_ptr, ipv6_address);
            server_address += String(ipv6_address);
            tr_debug("M2MNsdlInterface::bootstrap_done_callback - IPv6 Server address received %s", server_address.c_str());
        }

        server_uri += server_address;
        server_uri.push_back(':');
        server_uri += server_port;

        security->set_resource_value(M2MSecurity::M2MServerUri, server_uri);
        security->set_resource_value(M2MSecurity::BootstrapServer, 0);

        M2MSecurity::SecurityModeType security_mode = M2MSecurity::SecurityNotSet;

        switch(server_info->omalw_server_security) {
            case SEC_NOT_SET:
                security_mode = M2MSecurity::SecurityNotSet;
                break;
            case PSK:
                //Not supported at the moment
                break;
            case RPK:
                //Not supported at the moment
                break;
            case CERTIFICATE:
                security_mode = M2MSecurity::Certificate;
                break;
            case NO_SEC:
                security_mode = M2MSecurity::NoSecurity;
                break;
            }
        security->set_resource_value(M2MSecurity::SecurityMode,security_mode);

        //TODO: This is mandatory parameter for LWM2M server,
        // why is it missing from nsdl-c API ?
        security->set_resource_value(M2MSecurity::ShortServerID,1);

        // Check certiticates only if the mode is Certificate
        // else it is in NoSecurity Mode, Psk and Rsk are not supported.
        if(M2MSecurity::Certificate == security_mode) {
            omalw_certificate_list_t *certificates = sn_nsdl_get_certificates(_nsdl_handle);
            if(certificates) {
                security->set_resource_value(M2MSecurity::ServerPublicKey,certificates->certificate_ptr[0],certificates->certificate_len[0]);
                security->set_resource_value(M2MSecurity::PublicKey,certificates->certificate_ptr[1],certificates->certificate_len[1]);
                security->set_resource_value(M2MSecurity::Secretkey,certificates->own_private_key_ptr,certificates->own_private_key_len);
            } else {
                // Mode is certificate but certificates are missing so its bootstrap error.
                delete security;
                security = NULL;
            }
        }
    }
    if(security) {
        tr_debug("M2MNsdlInterface::bootstrap_done_callback - bootstrap_done");
        // Inform that bootstrap is done and LWM2M server object is available.
        _observer.bootstrap_done(security);
    } else {
        tr_error("M2MNsdlInterface::bootstrap_done_callback - bootstrap_error");
        // Bootstrap error inform to the application.
        _observer.bootstrap_error();
    }
#endif //YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
}

bool M2MNsdlInterface::process_received_data(uint8_t *data,
                                             uint16_t data_size,
                                             sn_nsdl_addr_s *address)
{
    tr_debug("M2MNsdlInterface::process_received_data( data size %d)", data_size);
    return (0 == sn_nsdl_process_coap(_nsdl_handle,
                                      data,
                                      data_size,
                                      address)) ? true : false;
}

void M2MNsdlInterface::stop_timers()
{
    tr_debug("M2MNsdlInterface::stop_timers()");
    if(_registration_timer) {
        _registration_timer->stop_timer();
    }
}

void M2MNsdlInterface::timer_expired(M2MTimerObserver::Type type)
{
    if(M2MTimerObserver::NsdlExecution == type) {
        sn_nsdl_exec(_nsdl_handle, _counter_for_nsdl);
        _counter_for_nsdl++;
    } else if(M2MTimerObserver::Registration == type) {
        tr_debug("M2MNsdlInterface::timer_expired - M2MTimerObserver::Registration - Send update registration");
        send_update_registration();
    }
}

void M2MNsdlInterface::observation_to_be_sent(M2MBase *object,
                                              uint16_t obs_number,
                                              m2m::Vector<uint16_t> changed_instance_ids,
                                              bool send_object)
{
    tr_debug("M2MNsdlInterface::observation_to_be_sent(), %s", object->uri_path().c_str());
    if(object) {
        M2MBase::BaseType type = object->base_type();
        if(type == M2MBase::Object) {
            send_object_observation((M2MObject*)object,
                                    obs_number,
                                    changed_instance_ids,
                                    send_object);
        } else if(type == M2MBase::ObjectInstance) {
            send_object_instance_observation((M2MObjectInstance*)object, obs_number);
        } else if(type == M2MBase::Resource) {
            send_resource_observation((M2MResource*)object, obs_number);
        }
    }
}

void M2MNsdlInterface::send_delayed_response(M2MBase *base)
{
    tr_debug("M2MNsdlInterface::send_delayed_response()");
    M2MResource *resource = NULL;
    if(base) {
        if(M2MBase::Resource == base->base_type()) {
            resource = (M2MResource *)base;
        }
        if(resource) {
            sn_coap_hdr_s * coap_response = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
            if(coap_response) {
                memset(coap_response,0,sizeof(sn_coap_hdr_s));

                coap_response->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
                coap_response->msg_code = COAP_MSG_CODE_RESPONSE_CONTENT;
                resource->get_delayed_token(coap_response->token_ptr,coap_response->token_len);

                uint32_t length = 0;
                resource->get_value(coap_response->payload_ptr, length);
                coap_response->payload_len = length;

                sn_nsdl_send_coap_message(_nsdl_handle, _nsdl_handle->nsp_address_ptr->omalw_address_ptr, coap_response);

                if(coap_response->payload_ptr) {
                   free(coap_response->payload_ptr);
                   coap_response->payload_ptr = NULL;
                }
                if(coap_response->token_ptr) {
                    free(coap_response->token_ptr);
                    coap_response->token_ptr = NULL;
                }
                free(coap_response);
                coap_response = NULL;
            }
        }
    }
}

void M2MNsdlInterface::resource_to_be_deleted(const String &resource_name)
{
    tr_debug("M2MNsdlInterface::resource_to_be_deleted(resource_name %s)", resource_name.c_str());
    delete_nsdl_resource(resource_name);
}

void M2MNsdlInterface::value_updated(M2MBase *base,
                                     const String &object_name)
{
    tr_debug("M2MNsdlInterface::value_updated()");
    if(base) {
        switch(base->base_type()) {
            case M2MBase::Object:
                create_nsdl_object_structure((M2MObject*)base);
            break;
            case M2MBase::ObjectInstance:
                create_nsdl_object_instance_structure((M2MObjectInstance*)base);
            break;
            case M2MBase::Resource: {
                    M2MResource* resource = (M2MResource*)base;
                    create_nsdl_resource_structure(resource,object_name,
                                               resource->supports_multiple_instances());
            }
            break;
            case M2MBase::ResourceInstance: {
                M2MResourceInstance* instance = (M2MResourceInstance*)base;
                create_nsdl_resource(instance,object_name);
            }
            break;
        }
    }
    _observer.value_updated(base);
}

void M2MNsdlInterface::remove_object(M2MBase *object)
{
    tr_debug("M2MNsdlInterface::remove_object()");
    M2MObject* rem_object = (M2MObject*)object;
    if(rem_object && !_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        int index = 0;
        for ( ; it != _object_list.end(); it++, index++ ) {
            if((*it) == rem_object) {
                _object_list.erase(index);
                break;
            }
        }
    }
    if(_object_list.empty()) {
        _object_list.clear();
    }
}

bool M2MNsdlInterface::create_nsdl_object_structure(M2MObject *object)
{
    tr_debug("M2MNsdlInterface::create_nsdl_object_structure()");
    bool success = false;
    if(object) {
        //object->set_under_observation(false,this);
        M2MObjectInstanceList instance_list = object->instances();
        tr_debug("M2MNsdlInterface::create_nsdl_object_structure - Object Instance count %d", instance_list.size());
        if(!instance_list.empty()) {
           M2MObjectInstanceList::const_iterator it;
           it = instance_list.begin();
           for ( ; it != instance_list.end(); it++ ) {
               // Create NSDL structure for all object instances inside
               success = create_nsdl_object_instance_structure(*it);
           }
        }
    }
    if((object->operation() != M2MBase::NOT_ALLOWED)) {
        success = create_nsdl_resource(object,object->name(),object->register_uri());
    }
    return success;
}

bool M2MNsdlInterface::create_nsdl_object_instance_structure(M2MObjectInstance *object_instance)
{
    tr_debug("M2MNsdlInterface::create_nsdl_object_instance_structure()");
    bool success = false;
    if( object_instance) {

        // Append object instance id to the object name.
        String object_name = object_instance->name();
        object_name.push_back('/');
        object_name.append_int(object_instance->instance_id());

        //object_instance->set_under_observation(false,this);

        M2MResourceList res_list = object_instance->resources();
        tr_debug("M2MNsdlInterface::create_nsdl_object_instance_structure - ResourceBase count %d", res_list.size());
        if(!res_list.empty()) {
            M2MResourceList::const_iterator it;
            it = res_list.begin();
            for ( ; it != res_list.end(); it++ ) {
                // Create NSDL structure for all resources inside
                success = create_nsdl_resource_structure(*it,object_name,
                                                         (*it)->supports_multiple_instances());
            }
        }
        if(object_instance->operation() != M2MBase::NOT_ALLOWED) {
            success = create_nsdl_resource(object_instance,object_name,object_instance->register_uri());
        }
    }
    return success;
}

bool M2MNsdlInterface::create_nsdl_resource_structure(M2MResource *res,
                                                      const String &object_name,
                                                      bool multiple_instances)
{
    tr_debug("M2MNsdlInterface::create_nsdl_resource_structure(object_name %s)", object_name.c_str());
    bool success = false;
    if(res) {
        // Append object name to the resource.
        // Take out the instance Id and append to the
        // resource name like "object/0/+ resource + / + 0"
        String res_name = object_name;
        if (strcmp(res_name.c_str(), res->uri_path().c_str()) != 0) {
            res_name.push_back('/');
            res_name.append(res->name().c_str(),res->name().length());
        }

        // if there are multiple instances supported
        // then add instance Id into creating resource path
        // else normal /object_id/object_instance/resource_id format.
        if(multiple_instances) {
            M2MResourceInstanceList res_list = res->resource_instances();
            tr_debug("M2MNsdlInterface::create_nsdl_resource_structure - ResourceInstance count %d", res_list.size());
            if(!res_list.empty()) {
                M2MResourceInstanceList::const_iterator it;
                it = res_list.begin();
                for ( ; it != res_list.end(); it++ ) {
                    String inst_name = res_name;
                    // Create NSDL structure for all resources inside
                    inst_name.push_back('/');
                    inst_name.append_int((*it)->instance_id());

                    success = create_nsdl_resource((*it),inst_name,(*it)->register_uri());
                }
                // Register the main Resource as well along with ResourceInstances
                success = create_nsdl_resource(res,res_name,res->register_uri());
            }
        } else {
            tr_debug("M2MNsdlInterface::create_nsdl_resource_structure - res_name %s", res_name.c_str());
            success = create_nsdl_resource(res,res_name,res->register_uri());
        }
    }
    return success;
}

bool M2MNsdlInterface::create_nsdl_resource(M2MBase *base, const String &name, bool publish_uri)
{
    tr_debug("M2MNsdlInterface::create_nsdl_resource(name %s)", name.c_str());
    bool success = false;
    uint8_t* buffer = 0;
    uint32_t length = 0;

    // Create the NSDL Resource Pointer...
    if(base) {
        sn_nsdl_resource_info_s* resource = sn_nsdl_get_resource(_nsdl_handle,
                                                                 name.length(),
                                                                 (uint8_t*)name.c_str());
        if(resource) {
            success = true;
            if(resource->mode == SN_GRS_STATIC) {
                if((M2MBase::Resource == base->base_type() ||
                   M2MBase::ResourceInstance == base->base_type()) &&
                   M2MBase::Static == base->mode()) {
                    M2MResourceInstance *res = (M2MResourceInstance*)base;
                    res->get_value(buffer,length);
                    if(resource->resource) {
                        memory_free(resource->resource);
                    }
                    resource->resource = buffer;
                    resource->resourcelen = length;
                    resource->publish_uri = publish_uri;
                    sn_nsdl_update_resource(_nsdl_handle,resource);
                }
            }
            // Update Resource access everytime for existing resource.
            resource->access = (sn_grs_resource_acl_e)base->operation();
        } else if(_resource) {
            base->set_under_observation(false,this);
            //TODO: implement access control
            // Currently complete access is given
            _resource->access = (sn_grs_resource_acl_e)base->operation();

            if((M2MBase::Resource == base->base_type() ||
                M2MBase::ResourceInstance == base->base_type()) &&
               M2MBase::Static == base->mode()) {
                M2MResourceInstance *res = (M2MResourceInstance*)base;
                // Static resource is updated
                _resource->mode = SN_GRS_STATIC;

                res->get_value(buffer,length);
                _resource->resource = buffer;
                _resource->resourcelen = length;
            }

            if(M2MBase::Dynamic == base->mode()){
              // Dynamic resource is updated
               _resource->mode = SN_GRS_DYNAMIC;
               _resource->sn_grs_dyn_res_callback = __nsdl_c_callback;
            } else {
               _resource->mode = SN_GRS_DIRECTORY;
            }

            if( _resource->path != NULL ){
                memory_free(_resource->path);
                _resource->path = NULL;
            }
            if(name.length() > 0 ){
                _resource->path = alloc_string_copy((uint8_t*)name.c_str(), name.length());
                if(_resource->path) {
                    _resource->pathlen = name.length();
                }
            }
            if(!base->resource_type().empty() && _resource->resource_parameters_ptr) {
                _resource->resource_parameters_ptr->resource_type_ptr =
                    alloc_string_copy((uint8_t*)base->resource_type().c_str(),
                          base->resource_type().length());
                if(_resource->resource_parameters_ptr->resource_type_ptr) {
                    _resource->resource_parameters_ptr->resource_type_len =
                           base->resource_type().length();
                }
            }
            if(!base->interface_description().empty() && _resource->resource_parameters_ptr) {
                _resource->resource_parameters_ptr->interface_description_ptr =
                    alloc_string_copy((uint8_t*)base->interface_description().c_str(),
                          base->interface_description().length());
                if(_resource->resource_parameters_ptr->interface_description_ptr) {
                    _resource->resource_parameters_ptr->interface_description_len =
                           base->interface_description().length();
                 }
            }
            if(_resource->resource_parameters_ptr) {
                _resource->resource_parameters_ptr->coap_content_type = base->coap_content_type();
                _resource->resource_parameters_ptr->observable = (uint8_t)base->is_observable();
            }
            _resource->publish_uri = publish_uri;
            int8_t result = sn_nsdl_create_resource(_nsdl_handle,_resource);
            tr_debug("M2MNsdlInterface::create_nsdl_resource - Creating in NSDL-C result %d", result);

            // Either the resource is created or it already
            // exists , then result is success.
            if (result == 0 ||
               result == -2){
                success = true;
            }

            if(_resource->path) {
                memory_free(_resource->path);
            }
            if(_resource->resource_parameters_ptr->resource_type_ptr){
                memory_free(_resource->resource_parameters_ptr->resource_type_ptr);
            }
            if(_resource->resource_parameters_ptr->interface_description_ptr){
                memory_free(_resource->resource_parameters_ptr->interface_description_ptr);
            }

            //Clear up the filled resource to fill up new resource.
            clear_resource(_resource);

            if(success) {
               base->set_under_observation(false,this);
            }
        }
    }
    if(buffer) {
        free(buffer);
    }
    return success;
}

// convenience method to get the URI from its buffer field...
String M2MNsdlInterface::coap_to_string(uint8_t *coap_data,int coap_data_length)
{
    String value = "";
    if (coap_data != NULL && coap_data_length > 0) {
        char buf[256+1];
        memset(buf,0,256+1);
        memcpy(buf,(char *)coap_data,coap_data_length);
        value = String(buf);
    }
    return value;
}

uint64_t M2MNsdlInterface::registration_time()
{
    uint64_t value = 0;
    if(_endpoint->lifetime_ptr) {
        value = atol((const char*)_endpoint->lifetime_ptr);
    }

    if(value >= OPTIMUM_LIFETIME) {
        value = value - REDUCE_LIFETIME;
    } else {
        value = REDUCTION_FACTOR * value;
    }
    tr_debug("M2MNsdlInterface::registration_time - value (in seconds) %ld", value);
    return value;
}

M2MBase* M2MNsdlInterface::find_resource(const String &object_name)
{
    M2MBase *object = NULL;
    if(!_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        for ( ; it != _object_list.end(); it++ ) {
            if((*it)->name() == object_name) {
                object = (*it);
                tr_debug("M2MNsdlInterface::find_resource(%s) found", object_name.c_str());
                break;
            }
            object = find_resource((*it),object_name);
            if(object != NULL) {
                tr_debug("M2MNsdlInterface::find_resource(%s) found", object_name.c_str());
                break;
            }
        }
    }
    return object;
}

M2MBase* M2MNsdlInterface::find_resource(const M2MObject *object,
                                         const String &object_instance)
{
    M2MBase *instance = NULL;
    if(object) {
        M2MObjectInstanceList list = object->instances();
        if(!list.empty()) {
            M2MObjectInstanceList::const_iterator it;
            it = list.begin();
            for ( ; it != list.end(); it++ ) {
                // Append object instance id to the object name.
                String name = (*it)->name();
                name.push_back('/');
                name.append_int((*it)->instance_id());

                if(name == object_instance){
                    instance = (*it);
                    break;
                }
                instance = find_resource((*it),object_instance);
                if(instance != NULL){
                    break;
                }
            }
        }
    }
    return instance;
}

M2MBase* M2MNsdlInterface::find_resource(const M2MObjectInstance *object_instance,
                                         const String &resource_instance)
{
    M2MBase *instance = NULL;
    if(object_instance) {
        M2MResourceList list = object_instance->resources();
        if(!list.empty()) {
            M2MResourceList::const_iterator it;
            it = list.begin();
            for ( ; it != list.end(); it++ ) {
                String name = object_instance->name();
                // Append object instance id to the object name.
                name.push_back('/');
                name.append_int(object_instance->instance_id());

                name.push_back('/');
                name+= (*it)->name();

                if(name == resource_instance) {
                    instance = *it;
                    break;
                } else if((*it)->supports_multiple_instances()) {
                    instance = find_resource((*it),name, resource_instance);
                    if(instance != NULL){
                        break;
                    }
                }
            }
        }
    }
    return instance;
}

M2MBase* M2MNsdlInterface::find_resource(const M2MResource *resource,
                                         const String &object_name,
                                         const String &resource_instance)
{
    M2MBase *res = NULL;
    if(resource) {
        if(resource->supports_multiple_instances()) {
            M2MResourceInstanceList list = resource->resource_instances();
            if(!list.empty()) {
                M2MResourceInstanceList::const_iterator it;
                it = list.begin();
                for ( ; it != list.end(); it++ ) {
                    String name = object_name;
                    // if there are multiple instances supported
                    // then add instance Id into creating resource path
                    // else normal /object_id/object_instance/resource_id format.

                    name.push_back('/');
                    name.append_int((*it)->instance_id());

                    if(name == resource_instance){
                        res = (*it);
                        break;
                    }
                }
            }
        }
    }
    return res;
}

bool M2MNsdlInterface::object_present(M2MObject* object) const
{
    bool success = false;
    if(object && !_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        for ( ; it != _object_list.end(); it++ ) {
            if((*it) == object) {
                success = true;
                break;
            }
        }
    }
    return success;
}

bool M2MNsdlInterface::add_object_to_list(M2MObject* object)
{
    bool success = false;
    if(object && !object_present(object)) {
        _object_list.push_back(object);
        success = true;
    }
    return success;
}

void M2MNsdlInterface::clear_resource(sn_nsdl_resource_info_s *&resource)
{
    //Clear up the filled resource to fill up new resource.
    if(resource && resource->resource_parameters_ptr) {
        sn_nsdl_resource_parameters_s *temp_resource_parameter = resource->resource_parameters_ptr;
        memset(resource->resource_parameters_ptr, 0, sizeof(sn_nsdl_resource_parameters_s));
        memset(resource,0, sizeof(sn_nsdl_resource_info_s));
        resource->resource_parameters_ptr = temp_resource_parameter;
    }
}

M2MInterface::Error M2MNsdlInterface::interface_error(sn_coap_hdr_s *coap_header)
{
    M2MInterface::Error error = M2MInterface::ErrorNone;
    if(coap_header) {
        switch(coap_header->msg_code) {
            case COAP_MSG_CODE_RESPONSE_BAD_REQUEST:
            case COAP_MSG_CODE_RESPONSE_BAD_OPTION:
            case COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_INCOMPLETE:
            case COAP_MSG_CODE_RESPONSE_PRECONDITION_FAILED:
            case COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE:
            case COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT:
                error = M2MInterface::InvalidParameters;
                break;
            case COAP_MSG_CODE_RESPONSE_UNAUTHORIZED:
            case COAP_MSG_CODE_RESPONSE_FORBIDDEN:
            case COAP_MSG_CODE_RESPONSE_NOT_ACCEPTABLE:
            case COAP_MSG_CODE_RESPONSE_NOT_FOUND:
            case COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED:
                error = M2MInterface::NotAllowed;
                break;
            case COAP_MSG_CODE_RESPONSE_CREATED:
            case COAP_MSG_CODE_RESPONSE_DELETED:
            case COAP_MSG_CODE_RESPONSE_VALID:
            case COAP_MSG_CODE_RESPONSE_CHANGED:
            case COAP_MSG_CODE_RESPONSE_CONTENT:
                error = M2MInterface::ErrorNone;
                break;
            default:
                error = M2MInterface::UnknownError;
                break;
        }
        if(coap_header->coap_status == COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED) {
            error = M2MInterface::NetworkError;
        }
    }
    return error;
}

void M2MNsdlInterface::send_object_observation(M2MObject *object,
                                               uint16_t obs_number,
                                               m2m::Vector<uint16_t> changed_instance_ids,
                                               bool send_object)
{
    tr_debug("M2MNsdlInterface::send_object_observation");
    if(object) {
        uint8_t *value = 0;
        uint32_t length = 0;
        uint8_t *token = 0;
        uint32_t token_length = 0;

        M2MTLVSerializer *serializer = new M2MTLVSerializer();
        if (serializer) {
            // Send whole object structure
            if (send_object) {
                value = serializer->serialize(object->instances(), length);
            }
            // Send only change object instances
            else {
                M2MObjectInstanceList list;
                Vector<uint16_t>::const_iterator it;
                it = changed_instance_ids.begin();
                for (; it != changed_instance_ids.end(); it++){
                    M2MObjectInstance* obj_instance = object->object_instance(*it);
                    if (obj_instance){
                        list.push_back(obj_instance);
                    }
                }
                if (!list.empty()) {
                    value = serializer->serialize(list, length);
                    list.clear();
                }
            }
        }
        delete serializer;

        object->get_observation_token(token,token_length);

        send_notification(token,
                          token_length,
                          value,
                          length,
                          obs_number,
                          object->max_age(),
                          object->coap_content_type(),
                          object->uri_path());

        memory_free(value);
        memory_free(token);
    }
}

void M2MNsdlInterface::send_object_instance_observation(M2MObjectInstance *object_instance,
                                                        uint16_t obs_number)
{
    tr_debug("M2MNsdlInterface::send_object_instance_observation");
    if(object_instance) {
        uint8_t *value = 0;
        uint32_t length = 0;
        uint8_t *token = 0;
        uint32_t token_length = 0;

        M2MTLVSerializer *serializer = new M2MTLVSerializer();
        if(serializer) {
            value = serializer->serialize(object_instance->resources(), length);
            delete serializer;
        }

        object_instance->get_observation_token(token,token_length);

        send_notification(token,
                          token_length,
                          value,
                          length,
                          obs_number,
                          object_instance->max_age(),
                          object_instance->coap_content_type(),
                          object_instance->uri_path());

        memory_free(value);
        memory_free(token);
    }
}

void M2MNsdlInterface::send_resource_observation(M2MResource *resource,
                                                 uint16_t obs_number)
{
    tr_debug("M2MNsdlInterface::send_resource_observation");
    if(resource) {
        uint8_t *value = 0;
        uint32_t length = 0;
        uint8_t *token = 0;
        uint32_t token_length = 0;

        resource->get_observation_token(token,token_length);
        uint8_t content_type = 0;
        if(M2MResourceInstance::OPAQUE == resource->resource_instance_type()) {
            content_type = COAP_CONTENT_OMA_OPAQUE_TYPE;
        }
        if (resource->resource_instance_count() > 0) {
            M2MTLVSerializer *serializer = new M2MTLVSerializer();
            content_type = COAP_CONTENT_OMA_TLV_TYPE;
            if(serializer) {
                value = serializer->serialize(resource, length);
                delete serializer;
            }
        } else {
            resource->get_value(value,length);
        }
        send_notification(token,
                          token_length,
                          value,
                          length,
                          obs_number,
                          resource->max_age(),
                          resource->coap_content_type(),
                          resource->uri_path());

        memory_free(value);
        memory_free(token);
    }
}

void M2MNsdlInterface::build_observation_number(uint8_t *obs_number,
                                                uint8_t *obs_len,
                                                uint16_t number)
{
    if(number > 0xFF) {
        *obs_len = 2;
        *(obs_number) = (number >> 8) & 0x00FF;
        obs_number[1] = number & 0x00FF;
    } else {
        *obs_len = 1;
        *(obs_number) = number & 0x00FF;
    }
}

void M2MNsdlInterface::send_notification(uint8_t *token,
                                         uint8_t  token_length,
                                         uint8_t *value,
                                         uint32_t value_length,
                                         uint16_t observation,
                                         uint32_t max_age,
                                         uint8_t  coap_content_type,
                                         const String  &uri_path)

{
    tr_debug("M2MNsdlInterface::send_notification");
    sn_coap_hdr_s *notification_message_ptr;

    /* Allocate and initialize memory for header struct */
    notification_message_ptr = (sn_coap_hdr_s *)memory_alloc(sizeof(sn_coap_hdr_s));
    if (notification_message_ptr) {
        memset(notification_message_ptr, 0, sizeof(sn_coap_hdr_s));

        notification_message_ptr->options_list_ptr = (sn_coap_options_list_s *)memory_alloc(sizeof(sn_coap_options_list_s));
        if (notification_message_ptr->options_list_ptr) {

            memset(notification_message_ptr->options_list_ptr , 0, sizeof(sn_coap_options_list_s));

            /* Fill header */
            notification_message_ptr->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
            notification_message_ptr->msg_code = COAP_MSG_CODE_RESPONSE_CONTENT;

            /* Fill token */
            notification_message_ptr->token_len = token_length;
            notification_message_ptr->token_ptr = token;

            /* Fill payload */
            notification_message_ptr->payload_len = value_length;
            notification_message_ptr->payload_ptr = value;

            /* Fill uri path */
            notification_message_ptr->uri_path_len = uri_path.size();
            notification_message_ptr->uri_path_ptr = (uint8_t *)uri_path.c_str();

            /* Fill observe */
            uint8_t observation_number[2];
            uint8_t observation_number_length = 0;

            build_observation_number(observation_number,
                                     &observation_number_length,
                                     observation);
            notification_message_ptr->options_list_ptr->observe_len = observation_number_length;
            notification_message_ptr->options_list_ptr->observe_ptr = observation_number;

            notification_message_ptr->options_list_ptr->max_age_ptr =
                    m2m::String::convert_integer_to_array(max_age,
                                                          notification_message_ptr->options_list_ptr->max_age_len);

            notification_message_ptr->content_type_ptr =
                    m2m::String::convert_integer_to_array(coap_content_type,
                                                          notification_message_ptr->content_type_len);

            /* Send message */
            sn_nsdl_send_coap_message(_nsdl_handle,
                                      _nsdl_handle->nsp_address_ptr->omalw_address_ptr,
                                      notification_message_ptr);

            /* Free memory */
            notification_message_ptr->uri_path_ptr = NULL;
            notification_message_ptr->payload_ptr = NULL;
            notification_message_ptr->options_list_ptr->observe_ptr = NULL;
            notification_message_ptr->token_ptr = NULL;
            free(notification_message_ptr->content_type_ptr);
            notification_message_ptr->content_type_ptr = NULL;
            free(notification_message_ptr->options_list_ptr->max_age_ptr);
            notification_message_ptr->options_list_ptr->max_age_ptr = NULL;
        }
        sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, notification_message_ptr);
    }
}
