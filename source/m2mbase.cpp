/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2mbase.h"
#include "lwm2m-client/m2mobservationhandler.h"
#include "lwm2m-client/m2mconstants.h"
#include "lwm2m-client/m2mtimer.h"
#include "include/m2mreporthandler.h"

M2MBase& M2MBase::operator=(const M2MBase& other)
{
    if (this != &other) { // protect against invalid self-assignment
        _operation = other._operation;
        _mode = other._mode;
        _name = other._name;
        _resource_type = other._resource_type;
        _interface_description = other._interface_description;
        _coap_content_type = other._coap_content_type;
        _instance_id = other._instance_id;
        _observable = other._observable;
        _observation_number = other._observation_number;
        _is_numeric = other._is_numeric;

        _observation_handler = other._observation_handler;

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

        if(_token) {
            free(_token);
            _token = NULL;
            _token_length = 0;
        }
        _token_length = other._token_length;
        if(other._token) {
            _token = (uint8_t *)malloc(other._token_length+1);
            if(_token) {
                memset(_token, 0, other._token_length+1);
                memcpy((uint8_t *)_token, (uint8_t *)other._token, other._token_length);
            }
        }

        if(_report_handler) {
            delete _report_handler;
            _report_handler = NULL;
        }
        if(other._report_handler) {
            _report_handler = new M2MReportHandler(*other._report_handler);
        }
    }
    return *this;
}

M2MBase::M2MBase(const M2MBase& other) :
    _report_handler(NULL),
    _value(NULL),
    _value_length(0),
    _token(NULL),
    _token_length(0)
{
    _operation = other._operation;
    _mode = other._mode;
    _name = other._name;
    _resource_type = other._resource_type;
    _interface_description = other._interface_description;
    _coap_content_type = other._coap_content_type;
    _instance_id = other._instance_id;
    _observable = other._observable;
    _observation_handler = other._observation_handler;
    _observation_number = other._observation_number;
    _is_numeric = other._is_numeric;
    _value_length = other._value_length;


    if(other._value) {
        _value = (uint8_t *)malloc(other._value_length+1);
        if(_value) {
            memset(_value, 0, other._value_length+1);
            memcpy((uint8_t *)_value, (uint8_t *)other._value, other._value_length);
        }
    }

    _token_length = other._token_length;
    if(other._token) {
        _token = (uint8_t *)malloc(other._token_length+1);
        if(_token) {
            memset(_token, 0, other._token_length+1);
            memcpy((uint8_t *)_token, (uint8_t *)other._token, other._token_length);
        }
    }

    if(other._report_handler) {
        _report_handler = new M2MReportHandler(*other._report_handler);
    }
}

M2MBase::M2MBase(const String & resource_name,
                 M2MBase::Mode mde)
: _report_handler(NULL),
  _observation_handler(NULL),
  _operation(M2MBase::NOT_ALLOWED),
  _mode(mde),
  _name(resource_name),
  _coap_content_type(0),
  _instance_id(0),
  _observable(false),
  _observation_number(0),
  _value(NULL),
  _value_length(0),
  _is_numeric(false),
  _token(NULL),
  _token_length(0)
{
}

M2MBase::~M2MBase()
{
    if(_report_handler) {
        delete _report_handler;
        _report_handler = NULL;
    }
    if(_value) {
        free(_value);
        _value = NULL;
        _value_length = 0;
    }
    if(_token) {
        free(_token);
        _token = NULL;
        _token_length = 0;
    }
}

void M2MBase::set_operation(M2MBase::Operation opr)
{
    // If the mode is Static, there is only GET_ALLOWED
   // supported.
    if(M2MBase::Static == _mode) {
            _operation = M2MBase::GET_ALLOWED;
    } else {
        _operation = opr;
    }
}

void M2MBase::set_interface_description(const String &desc)
{
    _interface_description = desc;
}

void M2MBase::set_resource_type(const String &res_type)
{
    _resource_type = res_type;
}

void M2MBase::set_coap_content_type(const uint8_t con_type)
{
    _coap_content_type = con_type;
}

void M2MBase::set_observable(bool observable)
{
   _observable = observable;
}

void M2MBase::set_under_observation(bool observed,
                                    M2MObservationHandler *handler)
{

    _observation_handler = handler;
    if(handler) {
        if(!_report_handler){
            _report_handler = new M2MReportHandler(*this);
        }
        _report_handler->set_under_observation(observed);
    } else {
        if(_report_handler) {
            delete _report_handler;
            _report_handler = NULL;
        }
    }

}

void M2MBase::set_observation_token(const uint8_t *token, const uint8_t length)
{
    if(_token) {
         free(_token);
         _token = NULL;
         _token_length = 0;
    }

    if( token != NULL && length > 0 ) {
        _token = (uint8_t *)malloc(length+1);
       if(_token) {
            memset(_token, 0, length+1);
            memcpy((uint8_t *)_token, (uint8_t *)token, length);
            _token_length = length;
        }
    }
}

void M2MBase::set_instance_id(const uint16_t inst_id)
{
    _instance_id = inst_id;
}

bool M2MBase::set_value(const uint8_t *value,
                        const uint32_t value_length,
                        bool  is_numeric)
{
    bool success = false;
    if(_value) {
         free(_value);
         _value = NULL;
         _value_length = 0;
    }

    if( value != NULL && value_length > 0 ) {
        success = true;
        _is_numeric = is_numeric;
        _value = (uint8_t *)malloc(value_length+1);
        if(_value) {
            memset(_value, 0, value_length+1);
            memcpy((uint8_t *)_value, (uint8_t *)value, value_length);
            _value_length = value_length;
            if(_is_numeric && _report_handler) {
                _report_handler->set_value(atof((const char*)_value));
            }
        }
    }
    return success;
}

void M2MBase::set_observation_number(const uint16_t observation_number)
{
    _observation_number = observation_number;
}

M2MBase::BaseType M2MBase::base_type() const
{
    return _base_type;
}

M2MBase::Operation M2MBase::operation() const
{
    return _operation;
}

const String& M2MBase::name() const
{
    return _name;
}

uint16_t M2MBase::instance_id() const
{
    return _instance_id;
}

const String& M2MBase::interface_description() const
{
    return _interface_description;
}

const String& M2MBase::resource_type() const
{
    return _resource_type;
}

uint8_t M2MBase::coap_content_type() const
{
    return _coap_content_type;
}

bool M2MBase::is_observable() const
{
    return _observable;
}

void M2MBase::get_observation_token(uint8_t *&token, uint32_t &token_length)
{
    token_length = 0;
    if(token) {
        free(token);
        token = NULL;
    }
    token = (uint8_t *)malloc(_token_length+1);
    if(token) {
        token_length = _token_length;
        memset(token, 0, _token_length+1);
        memcpy((uint8_t *)token, (uint8_t *)_token, token_length);
    }
}

M2MBase::Mode M2MBase::mode() const
{
    return _mode;
}

void M2MBase::get_value(uint8_t *&value, uint32_t &value_length)
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

uint16_t M2MBase::observation_number() const
{
    return _observation_number;
}

bool M2MBase::handle_observation_attribute(char *&query)
{
    bool success = false;
    if(_report_handler) {
        success = _report_handler->parse_notification_attribute(query,_base_type);
    }
    return success;
}

void M2MBase::observation_to_be_sent()
{
    if(_observation_handler) {
       _observation_handler->observation_to_be_sent(this);
    }
}

void M2MBase::set_base_type(M2MBase::BaseType type)
{
    _base_type = type;
}

void M2MBase::remove_resource_from_coap(const String &resource_name)
{
    if(_observation_handler) {
        _observation_handler->resource_to_be_deleted(resource_name);
    }
}

void M2MBase::remove_object_from_coap()
{
    if(_observation_handler) {
        _observation_handler->remove_object(this);
    }
}
