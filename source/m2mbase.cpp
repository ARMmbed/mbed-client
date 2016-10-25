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
#include "mbed-client/m2mbase.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mtimer.h"
#include "include/m2mreporthandler.h"
#include "include/nsdllinker.h"
#include "mbed-trace/mbed_trace.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define TRACE_GROUP "mClt"

M2MBase& M2MBase::operator=(const M2MBase& other)
{
    if (this != &other) { // protect against invalid self-assignment
        // tbd
        assert(0);
        //_operation = other._operation;
        //_mode = other._mode;
        //_coap_content_type = other._coap_content_type;
        //_instance_id = other._instance_id;
        //_observable = other._observable;
        _observation_number = other._observation_number;
        _observation_level = other._observation_level;
        _observation_handler = other._observation_handler;
        //_register_uri = other._register_uri;
        //_max_age = other._max_age;
        _is_under_observation = other._is_under_observation;

        free(_token);
        _token = NULL;

        _token_length = other._token_length;
        if(other._token) {
            _token = alloc_string_copy(other._token, other._token_length);
        }
       
        delete _report_handler;
        _report_handler = NULL;

        if(other._report_handler) {
            _report_handler = new M2MReportHandler(*other._report_handler);
        }
    }
    return *this;
}

M2MBase::M2MBase(const M2MBase& other) :
    //_observable(other._observable),
    //_register_uri(other._register_uri),
    _is_under_observation(other._is_under_observation),
    _token_length(other._token_length),
    //_operation(other._operation),
    //_mode(other._mode),
    _observation_level(other._observation_level),
    _observation_number(other._observation_number),
    //_instance_id(other._instance_id),
    //_max_age(other._max_age),
    _report_handler(NULL),    
    _token(NULL),
    #ifndef RAM_OPTIMIZED
    _function_pointer(NULL),
    #endif
    //_coap_content_type(other._coap_content_type),
    _observation_handler(other._observation_handler)
{
    // tbd
    assert(0);
 
    if(other._token) {
        _token = alloc_string_copy((uint8_t *)other._token, other._token_length);
    }

    if(other._report_handler) {
        _report_handler = new M2MReportHandler(*other._report_handler);
    }
}


M2MBase::M2MBase(const String resource_name,
                 M2MBase::Mode mde)
:
  _is_static(0),
  _is_under_observation(false),
  _token_length(0),
  _observation_level(M2MBase::None),
  _observation_number(0), 
  _report_handler(NULL),      
  _token(NULL),
  #ifndef RAM_OPTIMIZED
  _function_pointer(NULL),
  #endif
  _observation_handler(NULL)
{
    _sn_resource = (lwm2m_parameters_s*)memory_alloc(sizeof(lwm2m_parameters_s));
    if(_sn_resource) {
        memset(_sn_resource, 0, sizeof(lwm2m_parameters_s));
        _sn_resource->static_resource_params = (sn_nsdl_static_resource_parameters_s*)memory_alloc(sizeof(sn_nsdl_static_resource_parameters_s));
        if(_sn_resource->static_resource_params) {
            memset(_sn_resource->static_resource_params, 0, sizeof(sn_nsdl_static_resource_parameters_s));
            _sn_resource->static_resource_params->dynamic_resource_parameters = (sn_nsdl_dynamic_resource_parameters_s*)memory_alloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
            if(_sn_resource->static_resource_params->dynamic_resource_parameters) {
                memset(_sn_resource->static_resource_params->dynamic_resource_parameters, 0, sizeof(sn_nsdl_dynamic_resource_parameters_s));
            }
        }
    }

    _sn_resource->name = stringdup((char*)resource_name.c_str());
    _sn_resource->static_resource_params->mode = (const uint8_t)mde;
}

M2MBase::M2MBase(const lwm2m_parameters_s *s):
    _is_static(1),
    _is_under_observation(false),
    _token_length(0),
    _observation_level(M2MBase::None),
    _observation_number(0),
    _report_handler(NULL),
    _token(NULL),
    _observation_handler(NULL),
    _sn_resource((lwm2m_parameters_s*)s)
{

}

M2MBase::~M2MBase()
{
    delete _report_handler;
    free(_token);
    #ifndef RAM_OPTIMIZED
    delete _function_pointer;
    #endif
    free_resources();
}

void M2MBase::set_operation(M2MBase::Operation opr)
{
    // If the mode is Static, there is only GET_ALLOWED
    // supported.
    if(M2MBase::Static == mode()) {
        _sn_resource->static_resource_params->dynamic_resource_parameters->access = M2MBase::GET_ALLOWED;
    } else {
        _sn_resource->static_resource_params->dynamic_resource_parameters->access = opr;
    }
}

void M2MBase::set_interface_description(const String &description){
    if (!_is_static) {
        free(_sn_resource->static_resource_params->interface_description_ptr);
        _sn_resource->static_resource_params->interface_description_ptr = alloc_string_copy((uint8_t*)description.c_str(), description.length());
        _sn_resource->static_resource_params->interface_description_len = description.length();
    }
}

void M2MBase::set_resource_type(const String &resource_type){
    if (!_is_static) {
        free(_sn_resource->static_resource_params->resource_type_ptr);
        _sn_resource->static_resource_params->resource_type_ptr = alloc_string_copy((uint8_t*)resource_type.c_str(), resource_type.length());
        _sn_resource->static_resource_params->resource_type_len = resource_type.length();
    }
}

void M2MBase::set_coap_content_type(const uint8_t con_type)
{
    if (!_is_static) {
        _sn_resource->static_resource_params->dynamic_resource_parameters->coap_content_type = con_type;
    }
}

void M2MBase::set_observable(bool observable)
{
    if (!_is_static) {
        _sn_resource->static_resource_params->observable = observable;
    }
}

sn_nsdl_static_resource_parameters_s  * M2MBase::get_nsdl_resource()
{
 return _sn_resource->static_resource_params;
}


void M2MBase::add_observation_level(M2MBase::Observation obs_level)
{
    _observation_level = (M2MBase::Observation)(_observation_level | obs_level);
}

void M2MBase::remove_observation_level(M2MBase::Observation obs_level)
{
    _observation_level = (M2MBase::Observation)(_observation_level & ~obs_level);
}


// KS: this is always called wheather class is to be observerd or not.
// Anyhow when resources are freed, observationhandler must be alive!
void M2MBase::set_under_observation(bool observed,
                                    M2MObservationHandler *handler)
{

    tr_debug("M2MBase::set_under_observation - observed: %d", observed);
    tr_debug("M2MBase::set_under_observation - base_type: %d", base_type());
    _is_under_observation = observed;
    _observation_handler = handler;

    if(handler && observed) {
        if (base_type() != M2MBase::ResourceInstance) {
            if(!_report_handler){
                _report_handler = new M2MReportHandler(*this);
            }
            _report_handler->set_under_observation(observed);
        }
    } else {
        delete _report_handler;
        _report_handler = NULL;
    }
}

void M2MBase::set_observation_token(const uint8_t *token, const uint8_t length)
{
     free(_token);
     _token = NULL;
     _token_length = 0;

    if( token != NULL && length > 0 ) {
        _token = alloc_string_copy((uint8_t *)token, length);
        if(_token) {
            _token_length = length;
        }
    }
}


void M2MBase::set_instance_id(const uint16_t inst_id)
{
    if(!_is_static) {
        _sn_resource->instance_id = inst_id;
    }
}


void M2MBase::set_observation_number(const uint16_t /*observation_number*/)
{
}


M2MBase::BaseType M2MBase::base_type() const
{    
    return (BaseType)_sn_resource->base_type;
}

M2MBase::Operation M2MBase::operation() const
{
    return (Operation)_sn_resource->static_resource_params->dynamic_resource_parameters->access;
}

String M2MBase::name() const
{    
    if (NULL!=_sn_resource->name)
        return String ((const char*)_sn_resource->name);
    else {
        return String (""); // return NULL?
    }
}

int32_t M2MBase::name_id() const
{
    return _sn_resource->name_id;
}

uint16_t M2MBase::instance_id() const
{    
    return _sn_resource->instance_id;
}

String M2MBase::interface_description() const
{
   if (NULL!=_sn_resource->static_resource_params->interface_description_ptr) {
        return String((const char*)_sn_resource->static_resource_params->interface_description_ptr);
   }
   else {
       return String("");
   }
}

String M2MBase::resource_type() const
{
   if (NULL!=_sn_resource->static_resource_params->resource_type_ptr) {
        return String((const char*)_sn_resource->static_resource_params->resource_type_ptr);
   }
   else {
        return String("");
   }

}

uint8_t M2MBase::coap_content_type() const
{
    return _sn_resource->static_resource_params->dynamic_resource_parameters->coap_content_type;
}

bool M2MBase::is_observable() const
{
    return _sn_resource->static_resource_params->observable;
}

M2MBase::Observation M2MBase::observation_level() const
{
    return _observation_level;
}

void M2MBase::get_observation_token(uint8_t *&token, uint32_t &token_length)
{
    token_length = 0;
    free(token);

    token = alloc_string_copy((uint8_t *)_token, _token_length);
    if(token) {
        token_length = _token_length;
    }
}

M2MBase::Mode M2MBase::mode() const
{
    return (M2MBase::Mode)_sn_resource->static_resource_params->mode;
}

uint16_t M2MBase::observation_number() const
{
    return _observation_number;
}

uint32_t M2MBase::max_age() const
{    
    return _sn_resource->max_age;
}

bool M2MBase::handle_observation_attribute(char *&query)
{
    tr_debug("M2MBase::handle_observation_attribute - under observation(%d)", is_under_observation());
    bool success = false;
    if(_report_handler) {
        success = _report_handler->parse_notification_attribute(query,base_type());
        if (success) {
            if (is_under_observation()) {
                _report_handler->set_under_observation(true);
            }
         } else {
            _report_handler->set_default_values();
        }
    }
    return success;
}

void M2MBase::observation_to_be_sent(m2m::Vector<uint16_t> changed_instance_ids, bool send_object)
{
    //TODO: Move this to M2MResourceInstance
    if(_observation_handler) {
       _observation_number++;
       _observation_handler->observation_to_be_sent(this,
                                                    _observation_number,
                                                    changed_instance_ids,
                                                    send_object);
    }
}

void M2MBase::set_base_type(M2MBase::BaseType type)
{
    if (!_is_static) {
        _sn_resource->base_type = type;
    }
}

void M2MBase::remove_resource_from_coap(const String &resource_name)
{
    if(_observation_handler) {
        _observation_handler->resource_to_be_deleted(resource_name);
        free_resources();
    }
    else {
        assert(0);
    }
}

void M2MBase::remove_object_from_coap()
{
    if(_observation_handler) {
        _observation_handler->remove_object(this);
        free_resources();
    }
    else {
        assert(0);
    }
}

sn_coap_hdr_s* M2MBase::handle_get_request(nsdl_s */*nsdl*/,
                                           sn_coap_hdr_s */*received_coap_header*/,
                                           M2MObservationHandler */*observation_handler*/)
{
    //Handled in M2MResource, M2MObjectInstance and M2MObject classes
    return NULL;
}

sn_coap_hdr_s* M2MBase::handle_put_request(nsdl_s */*nsdl*/,
                                           sn_coap_hdr_s */*received_coap_header*/,
                                           M2MObservationHandler */*observation_handler*/,
                                           bool &)
{
    //Handled in M2MResource, M2MObjectInstance and M2MObject classes
    return NULL;
}

sn_coap_hdr_s* M2MBase::handle_post_request(nsdl_s */*nsdl*/,
                                            sn_coap_hdr_s */*received_coap_header*/,
                                            M2MObservationHandler */*observation_handler*/,
                                            bool &,
                                            sn_nsdl_addr_s *)
{
    //Handled in M2MResource, M2MObjectInstance and M2MObject classes
    return NULL;
}

void *M2MBase::memory_alloc(uint32_t size)
{
    if(size)
        return malloc(size);
    else
        return 0;
}

void M2MBase::memory_free(void *ptr)
{
    free(ptr);
}

// KS: get rid of these and use char*
uint8_t* M2MBase::alloc_string_copy(const uint8_t* source, uint32_t size)
{
    assert(source != NULL);

    uint8_t* result = (uint8_t*)memory_alloc(size + 1);
    if (result) {
        memcpy(result, source, size);
        result[size] = '\0';
    }
    return result;
}

uint8_t* M2MBase::alloc_copy(const uint8_t* source, uint32_t size)
{
    assert(source != NULL);

    uint8_t* result = (uint8_t*)memory_alloc(size);
    if (result) {
        memcpy(result, source, size);
    }
    return result;
}

char* M2MBase::stringdup(const char* s)
{
    const size_t len = strlen(s)+1;
    char *p2 = static_cast<char*>(malloc(len));
    assert(p2 != NULL);
    memcpy(p2, s, len);
    p2[len-1] = '\0';
    return p2;
}

void M2MBase::free_resources()
{
    if (!_is_static) {
        free(_sn_resource->static_resource_params->dynamic_resource_parameters);
        free(_sn_resource->static_resource_params->path);
        free(_sn_resource->static_resource_params->resource);
        free(_sn_resource->static_resource_params->resource_type_ptr);
        free(_sn_resource->static_resource_params->interface_description_ptr);
        free(_sn_resource->static_resource_params);
        free(_sn_resource);
    }
}

M2MReportHandler* M2MBase::report_handler()
{
    return _report_handler;
}

M2MObservationHandler* M2MBase::observation_handler()
{
    return _observation_handler;
}

void M2MBase::set_register_uri( bool register_uri)
{
    if (!_is_static) {
        _sn_resource->static_resource_params->publish_uri = register_uri;
    }
}

bool M2MBase::register_uri()
{
    return _sn_resource->static_resource_params->publish_uri;
}

bool M2MBase::is_integer(const String &value)
{
    const char *s = value.c_str();
    if(value.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) {
        return false;
    }
    char * p ;
    strtol(value.c_str(), &p, 10);
    return (*p == 0);
}

void M2MBase::set_uri_path(const String &path)
{
    if (!_is_static) {
        free(_sn_resource->static_resource_params->path);
        _sn_resource->static_resource_params->path = alloc_string_copy((uint8_t*)path.c_str(), path.length());
        _sn_resource->static_resource_params->pathlen = path.length();
    }
}

bool M2MBase::is_static() {

    return _is_static;
}

String M2MBase::uri_path() const
{    
    if(_sn_resource->static_resource_params->path!=NULL) {
        return String((const char*)_sn_resource->static_resource_params->path);
        }
    else {
        return String("");
    }
}

bool M2MBase::is_under_observation() const
{
    return _is_under_observation;
}

#ifndef RAM_OPTIMIZED
void M2MBase::set_value_updated_function(value_updated_callback callback)
{
    _value_updated_callback = callback;
}

void M2MBase::set_value_updated_function(value_updated_callback2 callback)
{
    delete _function_pointer;
    _function_pointer = new FP1<void, const char*>(callback);
    set_value_updated_function(value_updated_callback(_function_pointer,
                                                      &FP1<void, const char*>::call));
}

bool M2MBase::is_value_updated_function_set()
{
   return (_value_updated_callback) ? true : false;
}

void M2MBase::execute_value_updated(const String& name)
{
    if(_value_updated_callback) {
        _value_updated_callback(name.c_str());
    }
}
#endif
