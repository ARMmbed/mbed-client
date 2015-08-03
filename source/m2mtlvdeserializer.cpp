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
#include <stdio.h>
#include "include/m2mtlvdeserializer.h"
#include "lwm2m-client/m2mconstants.h"

M2MTLVDeserializer::M2MTLVDeserializer()
{
}

M2MTLVDeserializer::~M2MTLVDeserializer()
{
}

bool M2MTLVDeserializer::is_object_instance(uint8_t *tlv)
{
    return is_object_instance(tlv, 0);
}

bool M2MTLVDeserializer::is_resource(uint8_t *tlv)
{
    return is_resource(tlv, 0);
}

bool M2MTLVDeserializer::is_multiple_resource(uint8_t *tlv)
{
    return is_multiple_resource(tlv, 0);
}

bool M2MTLVDeserializer::is_resource_instance(uint8_t *tlv)
{
    return is_resource_instance(tlv, 0);
}

void M2MTLVDeserializer::deserialise_object_instances(uint8_t* tlv, uint32_t tlv_size, M2MObjectInstanceList &list)
{
    if (is_object_instance(tlv) ) {
        deserialize_object_instances(tlv, tlv_size, 0, list );
    }
}

void M2MTLVDeserializer::deserialize_resources(uint8_t *tlv, uint32_t tlv_size, M2MResourceList &list)
{
    if (!is_resource(tlv) && !is_multiple_resource(tlv)) {
        return ;
    }
    deserialize_resources(tlv, tlv_size, 0, list );
}
    

void M2MTLVDeserializer::deserialize_object_instances(uint8_t *tlv, uint32_t tlv_size, uint32_t offset, M2MObjectInstanceList &list)
{
    TypeIdLength *til = TypeIdLength::createTypeIdLength(tlv, offset)->deserialize();
    offset = til->_offset;

    M2MObjectInstanceList::const_iterator it;
    it = list.begin();

    if (til->_type == TYPE_OBJECT_INSTANCE) {
        for (; it!=list.end(); it++) {
            if((*it)->instance_id() == til->_id) {
                M2MResourceList resource_list = (*it)->resources();
                deserialize_resources(tlv, tlv_size-offset-til->_length, 0, resource_list);
            }
        }
    } else {
        return;
    }

    offset += til->_length;

    if(offset < tlv_size) {
        deserialize_object_instances(tlv, tlv_size-offset, offset, list);
    }
    delete til;
}

void M2MTLVDeserializer::deserialize_resources(uint8_t *tlv, uint32_t tlv_size, uint32_t offset, M2MResourceList &list)
{
    TypeIdLength *til = TypeIdLength::createTypeIdLength(tlv, offset)->deserialize();
    offset = til->_offset;

    M2MResourceList::const_iterator it;
    it = list.begin();

    if (til->_type == TYPE_RESOURCE) {
        for (; it!=list.end(); it++) {
            if((*it)->name_id() == til->_id){
                (*it)->set_value(tlv+offset, til->_length);
                break;
            }
        }
    } else if (til->_type == TYPE_MULTIPLE_RESOURCE) {
        for (; it!=list.end(); it++) {
            if((*it)->supports_multiple_instances()) {
                M2MResourceInstanceList instance_list = (*it)->resource_instances();
                deserialize_resource_instances(tlv, tlv_size-offset, offset, instance_list);
            }
        }

    } else {
        delete til;
        return;
    }
    offset += til->_length;

    delete til;
    if(offset < tlv_size) {
        deserialize_resources(tlv, tlv_size-offset, offset, list);
    }
}

void M2MTLVDeserializer::deserialize_resource_instances(uint8_t *tlv, uint32_t tlv_size, uint32_t offset, M2MResourceInstanceList &list)
{
    TypeIdLength *til = TypeIdLength::createTypeIdLength(tlv, offset)->deserialize();
    offset = til->_offset;

    if (til->_type == TYPE_RESOURCE_INSTANCE) {
        M2MResourceInstanceList::const_iterator it;
        it = list.begin();
        for (; it!=list.end(); it++) {
            if((*it)->instance_id() == til->_id) {
                (*it)->set_value(tlv+offset, til->_length);
                 break;
            }
        }
    } else {
        delete til;
        return;
    }

    offset += til->_length;

    delete til;
    if(offset < tlv_size) {
        deserialize_resource_instances(tlv, tlv_size-offset, offset, list);
    }

}

bool M2MTLVDeserializer::is_object_instance(uint8_t *tlv, uint32_t offset)
{
    return (tlv[offset] & TYPE_OBJECT_INSTANCE) == TYPE_OBJECT_INSTANCE;
}

bool M2MTLVDeserializer::is_resource(uint8_t *tlv, uint32_t offset)
{
    return (tlv[offset] & TYPE_RESOURCE) == TYPE_RESOURCE;
}
    
bool M2MTLVDeserializer::is_multiple_resource(uint8_t *tlv, uint32_t offset)
{
    return (tlv[offset] & TYPE_MULTIPLE_RESOURCE) == TYPE_MULTIPLE_RESOURCE;
}
    
bool M2MTLVDeserializer::is_resource_instance(uint8_t *tlv, uint32_t offset)
{
    return (tlv[offset] & TYPE_RESOURCE_INSTANCE) == TYPE_RESOURCE_INSTANCE;
}

TypeIdLength* TypeIdLength::createTypeIdLength(uint8_t *tlv, uint32_t offset)
{
    TypeIdLength *til = new TypeIdLength();
    til->_tlv = tlv;
    til->_offset = offset;
    til->_type = tlv[offset] & 0xC0;
    til->_id = -1;
    til->_length = 0;
    return til;
}

TypeIdLength* TypeIdLength::deserialize()
{
    uint32_t idLength = _tlv[_offset] & ID16;
    uint32_t lengthType = _tlv[_offset] & LENGTH24;
    if (lengthType == 0) {
        _length = _tlv[_offset] & 0x07;
    }
    _offset++;

    deserialiseID(idLength);
    deserialiseLength(lengthType);

    return this;
}

void TypeIdLength::deserialiseID(uint32_t idLength)
{
    _id = _tlv[_offset++] & 0xFF;
    if (idLength == ID16) {
        _id = (_id << 8) + (_tlv[_offset++] & 0xFF);
    }
}

void TypeIdLength::deserialiseLength(uint32_t lengthType)
{
    if (lengthType > 0) {
        _length = _tlv[_offset++] & 0xFF;
    }
    if (lengthType > LENGTH8) {
        _length = (_length << 8) + (_tlv[_offset++] & 0xFF);
    }
    if (lengthType > LENGTH16) {
        _length = (_length << 8) + (_tlv[_offset++] & 0xFF);
    }
}
