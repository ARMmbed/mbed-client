#include "m2mresource.h"
#include "m2mconstants.h"

M2MResource& M2MResource::operator=(const M2MResource& other)
{
    return *this;
}

M2MResource::M2MResource(const M2MResource& other)
: M2MBase(other)
{
    *this = other;
}

M2MResource::M2MResource(const String &res_name,
                         const String &res_type,
                         M2MBase::Mode resource_mode)
: M2MBase(res_name,
          resource_mode)
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
