/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2msecurity_stub.h"

bool m2msecurity_stub::has_value;
uint32_t m2msecurity_stub::int_value;
bool m2msecurity_stub::bool_value;
String *m2msecurity_stub::string_value;
M2MResource* m2msecurity_stub::resource;

void m2msecurity_stub::clear()
{
    has_value = false;
    int_value = -1;
    bool_value = false;
    string_value = NULL;
    resource = NULL;
}

M2MSecurity::M2MSecurity(ServerType ser_type)
: M2MObject("0"),
 _server_type(ser_type),
 _server_instance(NULL)
{
}

M2MSecurity::~M2MSecurity()
{
}

M2MResource* M2MSecurity::create_resource(SecurityResource, uint32_t)
{
    return m2msecurity_stub::resource;
}

bool M2MSecurity::delete_resource(SecurityResource)
{
    return m2msecurity_stub::bool_value;
}

bool M2MSecurity::set_resource_value(SecurityResource,
                                     const String &)
{
    return m2msecurity_stub::bool_value;
}

bool M2MSecurity::set_resource_value(SecurityResource,
                                     uint32_t)
{
    return m2msecurity_stub::bool_value;
}

bool M2MSecurity::set_resource_value(SecurityResource,
                                     const uint8_t *,
                                     const uint16_t)
{
    return m2msecurity_stub::bool_value;
}

String M2MSecurity::resource_value_string(SecurityResource) const
{
    return *m2msecurity_stub::string_value;
}

uint32_t M2MSecurity::resource_value_buffer(SecurityResource,
                               uint8_t *&value) const
{
    if( m2msecurity_stub::has_value ){
        value = (uint8_t *)malloc(5);
        return 5;
    }
    return m2msecurity_stub::int_value;
}


uint32_t M2MSecurity::resource_value_int(SecurityResource) const
{
    return m2msecurity_stub::int_value;
}

bool M2MSecurity::is_resource_present(SecurityResource) const
{
    return m2msecurity_stub::bool_value;
}

uint16_t M2MSecurity::total_resource_count() const
{
    return (uint16_t)m2msecurity_stub::int_value;
}

M2MSecurity::ServerType M2MSecurity::server_type() const
{
    return _server_type;
}
