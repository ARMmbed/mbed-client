/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2mresource.h"
#include "lwm2m-client/m2mconstants.h"

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
    *this = other;
}

M2MResource::M2MResource(const String &res_name,
                         const String &res_type,
                         M2MBase::Mode resource_mode,
                         bool multiple_instances)
: M2MBase(res_name,
          resource_mode),
  _has_multiple_instances(multiple_instances)
{
    M2MBase::set_resource_type(res_type);
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
    return _has_multiple_instances;
}
