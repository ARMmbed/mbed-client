/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mresource_stub.h"

uint32_t m2mresource_stub::int_value;
bool m2mresource_stub::bool_value;
M2MResourceInstanceList m2mresource_stub::list;
M2MResourceInstance *m2mresource_stub::instance;

void m2mresource_stub::clear()
{
    int_value = 0;
    bool_value = false;
    list.clear();
    instance = NULL;
}

M2MResource& M2MResource::operator=(const M2MResource& other)
{
    return *this;
}

M2MResource::M2MResource(const M2MResource& other)
: M2MResource(other)
{
    *this = other;
}

M2MResource::M2MResource(const String &resource_name,
                         const String &resource_type,
                         M2MResourceInstance::ResourceType type,
                         const uint8_t *value,
                         const uint8_t value_length,
                         bool)
: M2MResourceInstance(resource_name, resource_type, type, value, value_length)
{
}

M2MResource::M2MResource(const String &resource_name,
                         const String &resource_type,
                         M2MResourceInstance::ResourceType type,
                         bool,
                         bool )
: M2MResourceInstance(resource_name, resource_type, type)
{
}

M2MResource::~M2MResource()
{
}

bool M2MResource::supports_multiple_instances() const
{
    return m2mresource_stub::bool_value;
}

bool M2MResource::remove_resource_instance(uint16_t inst_id)
{
    return m2mresource_stub::bool_value;
}

M2MResourceInstance* M2MResource::resource_instance(uint16_t inst_id) const
{
    return m2mresource_stub::instance;
}

const M2MResourceInstanceList& M2MResource::resource_instances() const
{
    return m2mresource_stub::list;
}

uint16_t M2MResource::resource_instance_count() const
{
    return m2mresource_stub::int_value;
}

bool M2MResource::handle_observation_attribute(char *&query)
{
    return m2mresource_stub::bool_value;
}

void M2MResource::add_resource_instance(M2MResourceInstance *)
{
}
