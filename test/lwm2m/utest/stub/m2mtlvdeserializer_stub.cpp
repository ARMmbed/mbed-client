#include "m2mtlvdeserializer_stub.h"

bool m2mtlvdeserializer_stub::bool_value;

void m2mtlvdeserializer_stub::clear()
{
    bool_value = false;
}

M2MTLVDeserializer::M2MTLVDeserializer()
{
}

M2MTLVDeserializer::~M2MTLVDeserializer()
{
}

bool M2MTLVDeserializer::is_object_instance(uint8_t *)
{
    return m2mtlvdeserializer_stub::bool_value;
}

bool M2MTLVDeserializer::is_resource(uint8_t *)
{
    return m2mtlvdeserializer_stub::bool_value;
}

bool M2MTLVDeserializer::is_multiple_resource(uint8_t *)
{
    return m2mtlvdeserializer_stub::bool_value;
}

bool M2MTLVDeserializer::is_resource_instance(uint8_t *)
{
    return m2mtlvdeserializer_stub::bool_value;
}

void M2MTLVDeserializer::deserialise_object_instances(uint8_t* , uint32_t , M2MObjectInstanceList &)
{
}

void M2MTLVDeserializer::deserialize_resources(uint8_t *, uint32_t , M2MResourceList &list)
{
}

TypeIdLength* TypeIdLength::createTypeIdLength(uint8_t *, uint32_t)
{
    TypeIdLength *til = new TypeIdLength();
    return til;
}

TypeIdLength* TypeIdLength::deserialize()
{
    return this;
}

void TypeIdLength::deserialiseID(uint32_t idLength)
{
}

void TypeIdLength::deserialiseLength(uint32_t lengthType)
{
}
