/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mresourceinstance_stub.h"

uint32_t m2mresourceinstance_stub::int_value;
bool m2mresourceinstance_stub::bool_value;
M2MResourceInstance::ResourceType m2mresourceinstance_stub::resource_type;
sn_coap_hdr_s *m2mresourceinstance_stub::header;
uint8_t* m2mresourceinstance_stub::value;


void m2mresourceinstance_stub::clear()
{
    int_value = 0;
    bool_value = false;
    resource_type = M2MResourceInstance::STRING;
    header = NULL;
    value = NULL;
}

M2MResourceInstance& M2MResourceInstance::operator=(const M2MResourceInstance&)
{
    return *this;
}

M2MResourceInstance::M2MResourceInstance(const M2MResourceInstance& other)
: M2MBase(other)
{
    this->operator=(other);
}

M2MResourceInstance::M2MResourceInstance(const String &res_name,
                                         const String &,
                                         M2MResourceInstance::ResourceType)
: M2MBase(res_name,
          M2MBase::Dynamic)
{
}

M2MResourceInstance::M2MResourceInstance(const String &res_name,
                                         const String &,
                                         M2MResourceInstance::ResourceType,
                                         const uint8_t *,
                                         const uint8_t)
: M2MBase(res_name,
          M2MBase::Static)
{
}

M2MResourceInstance::~M2MResourceInstance()
{
}

M2MBase::BaseType M2MResourceInstance::base_type() const
{
    return M2MBase::Resource;
}

M2MResourceInstance::ResourceType M2MResourceInstance::resource_instance_type() const
{
    return _resource_type;
}

bool M2MResourceInstance::handle_observation_attribute(char *&)
{
    return m2mresourceinstance_stub::bool_value;
}

void M2MResourceInstance::set_execute_function(execute_callback)
{
}

bool M2MResourceInstance::set_value(const uint8_t *,
                                    const uint32_t )
{
    return m2mresourceinstance_stub::bool_value;
}

void M2MResourceInstance::execute(void *)
{
}

void M2MResourceInstance::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    value = (uint8_t *)malloc(m2mresourceinstance_stub::int_value);
    if(value) {
        value_length = m2mresourceinstance_stub::int_value;
        memcpy((uint8_t *)value, (uint8_t *)m2mresourceinstance_stub::value, value_length);
    }
}

uint8_t* M2MResourceInstance::value() const
{
    return m2mresourceinstance_stub::value;
}

uint32_t M2MResourceInstance::value_length() const
{
    return m2mresourceinstance_stub::int_value;
}

sn_coap_hdr_s* M2MResourceInstance::handle_get_request(nsdl_s *,
                                               sn_coap_hdr_s *,
                                               M2MObservationHandler *)
{
    return m2mresourceinstance_stub::header;
}

sn_coap_hdr_s* M2MResourceInstance::handle_put_request(nsdl_s *,
                                               sn_coap_hdr_s *,
                                               M2MObservationHandler *)
{
    return m2mresourceinstance_stub::header;
}

sn_coap_hdr_s* M2MResourceInstance::handle_post_request(nsdl_s *,
                                                sn_coap_hdr_s *,
                                                M2MObservationHandler *)
{
    return m2mresourceinstance_stub::header;
}
