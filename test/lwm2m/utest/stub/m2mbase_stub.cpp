/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mbase_stub.h"

uint8_t m2mbase_stub::uint8_value;
uint16_t m2mbase_stub::uint16_value;
uint16_t m2mbase_stub::int_value;
bool m2mbase_stub::bool_value;
String *m2mbase_stub::string_value;
uint8_t* m2mbase_stub::value;

M2MBase::BaseType m2mbase_stub::base_type;
M2MBase::Operation m2mbase_stub::operation;
M2MBase::Mode m2mbase_stub::mode_value;

void m2mbase_stub::clear()
{
    int_value = 0;
    uint8_value = 0;
    uint16_value = 0;
    string_value = NULL;
    mode_value = M2MBase::Static;
    base_type = M2MBase::Object;
    bool_value = false;
    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;
    if(value != NULL){
        delete value;
        value = NULL;
    }
}

M2MBase::M2MBase(const String &/*resource_name*/,
                 M2MBase::Mode /*mode*/)
{    
}

M2MBase& M2MBase::operator=(const M2MBase& other)
{
    if (this != &other) { // protect against invalid self-assignment
    }
    return *this;
}

M2MBase::M2MBase(const M2MBase& other)
{
    this->operator=(other);
}

M2MBase::~M2MBase()
{
}

void M2MBase::set_operation(M2MBase::Operation opr)
{
    m2mbase_stub::operation = opr;
}

void M2MBase::set_interface_description(const String &/*desc*/)
{
}

void M2MBase::set_resource_type(const String &/*res_type*/)
{    
}

void M2MBase::set_coap_content_type(const uint8_t /*con_type*/)
{
}

void M2MBase::set_instance_id(const uint16_t /*inst_id*/)
{
}

bool M2MBase::set_value(const uint8_t *, const uint32_t, bool is_numeric)
{
    return m2mbase_stub::bool_value;
}

void M2MBase::set_observation_number(const uint16_t /*observation_number*/)
{
}

M2MBase::Operation M2MBase::operation() const
{
    return m2mbase_stub::operation;
}

const String& M2MBase::name() const
{
    return *m2mbase_stub::string_value;
}

uint16_t M2MBase::instance_id() const
{
    return m2mbase_stub::int_value;
}

const String& M2MBase::interface_description() const
{
    return *m2mbase_stub::string_value;
}

const String& M2MBase::resource_type() const
{
    return *m2mbase_stub::string_value;
}

uint8_t M2MBase::coap_content_type() const
{
    return m2mbase_stub::uint8_value;
}

void M2MBase::set_observable(bool /*observable*/)
{
}

void M2MBase::set_under_observation(bool /*observed*/,
                                   M2MObservationHandler */*handler*/)
{
}

void M2MBase::set_observation_token(const uint8_t */*token*/,
                                    const uint8_t /*length*/)
{
}

bool M2MBase::is_observable() const
{
    return m2mbase_stub::bool_value;
}

void M2MBase::get_observation_token(uint8_t *&/*token*/,
                                    uint32_t &/*length*/)
{    
}

void M2MBase::set_base_type(M2MBase::BaseType /*type*/)
{
}

M2MBase::BaseType M2MBase::base_type() const
{
    return m2mbase_stub::base_type;
}

M2MBase::Mode M2MBase::mode() const
{
    return m2mbase_stub::mode_value;
}

void M2MBase::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    value = (uint8_t *)malloc(m2mbase_stub::int_value);
    if(value) {
        value_length = m2mbase_stub::int_value;
        memcpy((uint8_t *)value, (uint8_t *)m2mbase_stub::value, value_length);
    }
}

uint16_t M2MBase::observation_number() const
{
    return m2mbase_stub::uint16_value;
}

void M2MBase::remove_resource_from_coap(const String &)
{
}

void M2MBase::remove_object_from_coap()
{
}

bool M2MBase::handle_observation_attribute(char *&query)
{
    return m2mbase_stub::bool_value;
}

void M2MBase::observation_to_be_sent()
{
}
