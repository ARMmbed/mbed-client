/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mobject_stub.h"

u_int16_t m2mobject_stub::int_value;
bool m2mobject_stub::bool_value;
M2MObjectInstance* m2mobject_stub::inst;
M2MObjectInstanceList m2mobject_stub::instance_list;
M2MBase::BaseType m2mobject_stub::base_type;

void m2mobject_stub::clear()
{
    int_value = 0;
    bool_value = false;
    m2mobject_stub::base_type = M2MBase::Object;
    instance_list.clear();
}

M2MObject::M2MObject(const String &object_name)
: M2MBase(object_name,M2MBase::Dynamic)
{
}

M2MObject::~M2MObject()
{
}

M2MObject& M2MObject::operator=(const M2MObject& )
{
    return *this;
}

M2MObject::M2MObject(const M2MObject& other)
: M2MBase(other)
{
    *this = other;
}

M2MObjectInstance* M2MObject::create_object_instance()
{
    return m2mobject_stub::inst;
}

bool M2MObject::remove_object_instance(uint16_t)
{
    return m2mobject_stub::bool_value;
}

M2MObjectInstance* M2MObject::object_instance(uint16_t) const
{
    return m2mobject_stub::inst;
}

const M2MObjectInstanceList& M2MObject::instances() const
{
    return m2mobject_stub::instance_list;
}

uint16_t M2MObject::instance_count() const
{
    return m2mobject_stub::int_value;
}

M2MBase::BaseType M2MObject::base_type() const
{
    return m2mobject_stub::base_type;
}

bool M2MObject::handle_observation_attribute(char *&){
    return m2mobject_stub::bool_value;
}

