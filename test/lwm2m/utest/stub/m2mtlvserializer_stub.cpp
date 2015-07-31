#include "m2mtlvserializer_stub.h"

bool m2mtlvserializer_stub::bool_value;
uint8_t* m2mtlvserializer_stub::uint8_value;

void m2mtlvserializer_stub::clear()
{
    bool_value = false;
    uint8_value = NULL;
}

M2MTLVSerializer::M2MTLVSerializer()
{
}

M2MTLVSerializer::~M2MTLVSerializer()
{
}

uint8_t* M2MTLVSerializer::serialize(M2MObjectInstanceList object_instance_list, uint32_t &)
{
    return m2mtlvserializer_stub::uint8_value;
}

uint8_t* M2MTLVSerializer::serialize(M2MResourceList resource_list, uint32_t &size)
{
    return m2mtlvserializer_stub::uint8_value;
}

uint8_t* M2MTLVSerializer::serialize_object_instances(M2MObjectInstanceList object_instance_list, uint32_t &size)
{
    return m2mtlvserializer_stub::uint8_value;
}

uint8_t* M2MTLVSerializer::serialize_resources(M2MResourceList resource_list, uint32_t &size)
{
    return m2mtlvserializer_stub::uint8_value;
}
