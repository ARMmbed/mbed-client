/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
