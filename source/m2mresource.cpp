/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2mresource.h"
#include "lwm2m-client/m2mconstants.h"
#include "ns_trace.h"

M2MResource& M2MResource::operator=(const M2MResource& other)
{
    if(this != &other) {
    _has_multiple_instances = other._has_multiple_instances;
    }
    return *this;
}

M2MResource::M2MResource(const M2MResource& other)
: M2MBase(other)
{
    this->operator=(other);
}

M2MResource::M2MResource(const String &res_name,
                         const String &res_type,
                         M2MBase::Mode resource_mode,
                         bool multiple_instances)
: M2MBase(res_name,
          resource_mode),
  _has_multiple_instances(multiple_instances),
  _execute_callback(NULL)
{
    M2MBase::set_resource_type(res_type);
    M2MBase::set_base_type(M2MBase::Resource);
}

M2MResource::~M2MResource()
{    
}

M2MBase::BaseType M2MResource::base_type() const
{
    return M2MBase::base_type();
}

bool M2MResource::supports_multiple_instances() const
{
    return _has_multiple_instances;
}

bool M2MResource::handle_observation_attribute(char *&query)
{
    tr_debug("M2MResource::handle_observation_attribute()");
    return M2MBase::handle_observation_attribute(query);
}

void M2MResource::set_execute_function(execute_callback callback)
{
    _execute_callback = callback;
}

void M2MResource::execute(void *arguments)
{
    tr_debug("M2MResource::execute");
    if(_execute_callback) {
        _execute_callback(arguments);
    }
}
