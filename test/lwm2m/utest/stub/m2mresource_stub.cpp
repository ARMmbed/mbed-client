/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mresource_stub.h"

uint32_t m2mresource_stub::int_value;
bool m2mresource_stub::bool_value;

void m2mresource_stub::clear()
{
    int_value = 0;
    bool_value = false;
}

M2MResource::M2MResource(const String &res_name,
                         const String &/*res_type*/,
                         M2MBase::Mode res_mode,
                         bool /*multiple_instances*/)
: M2MBase(res_name,
          res_mode)
{
}

M2MResource& M2MResource::operator=(const M2MResource& )
{
    return *this;
}

M2MResource::M2MResource(const M2MResource& other)
: M2MBase(other)
{
    *this = other;
}

M2MResource::~M2MResource()
{
}

M2MBase::BaseType M2MResource::base_type() const
{
    return M2MBase::Resource;
}

bool M2MResource::supports_multiple_instances() const
{
    return m2mresource_stub::bool_value;
}

void M2MResource::set_execute_function(execute_callback )
{

}

bool M2MResource::handle_observation_attribute(char *&){
    return m2mresource_stub::bool_value;
}

void M2MResource::execute(void *)
{

}
