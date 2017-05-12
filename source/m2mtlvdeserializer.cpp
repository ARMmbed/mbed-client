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
#include "include/m2mtlvdeserializer.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-trace/mbed_trace.h"

#define TRACE_GROUP "mClt"
#define BUFFER_SIZE 10

bool M2MTLVDeserializer::is_object_instance(const uint8_t *tlv)
{
    return is_object_instance(tlv, 0);
}

bool M2MTLVDeserializer::is_resource(const uint8_t *tlv)
{
    return is_resource(tlv, 0);
}

bool M2MTLVDeserializer::is_multiple_resource(const uint8_t *tlv)
{
    return is_multiple_resource(tlv, 0);
}

bool M2MTLVDeserializer::is_resource_instance(const uint8_t *tlv)
{
    return is_resource_instance(tlv, 0);
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialise_object_instances(const uint8_t* tlv,
                                                                           uint32_t tlv_size,
                                                                           M2MObject &object,
                                                                           M2MTLVDeserializer::Operation operation)
{
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    if (is_object_instance(tlv) ) {
        tr_debug("M2MTLVDeserializer::deserialise_object_instances");
        error = deserialize_object_instances(tlv, tlv_size, 0, object,operation,false);
        if(M2MTLVDeserializer::None == error) {
            error = deserialize_object_instances(tlv, tlv_size, 0, object,operation,true);
        }
    } else {
        tr_debug("M2MTLVDeserializer::deserialise_object_instances ::NotValid");
        error = M2MTLVDeserializer::NotValid;
    }
    return error;
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialize_resources(const uint8_t *tlv,
                                                                    uint32_t tlv_size,
                                                                    M2MObjectInstance &object_instance,
                                                                    M2MTLVDeserializer::Operation operation)
{
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    if (!is_resource(tlv) && !is_multiple_resource(tlv)) {
        error = M2MTLVDeserializer::NotValid;
    } else {
        error = deserialize_resources(tlv, tlv_size, 0, object_instance, operation,false);
        if(M2MTLVDeserializer::None == error) {
            error = deserialize_resources(tlv, tlv_size, 0, object_instance, operation,true);
        }
    }
    return error;
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialize_resource_instances(const uint8_t *tlv,
                                                                             uint32_t tlv_size,
                                                                             M2MResource &resource,
                                                                             M2MTLVDeserializer::Operation operation)
{
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    if (!is_multiple_resource(tlv)) {
        error = M2MTLVDeserializer::NotValid;
    } else {
        tr_debug("M2MTLVDeserializer::deserialize_resource_instances()");
        uint8_t offset = 2;

        ((tlv[0] & 0x20) == 0) ? offset : offset++;

        uint8_t length = tlv[0] & 0x18;
        if(length == 0x08) {
            offset += 1;
        } else if(length == 0x10) {
            offset += 2;
        } else if(length == 0x18) {
            offset += 3;
        }

        tr_debug("M2MTLVDeserializer::deserialize_resource_instances() Offset %d", offset);
        error = deserialize_resource_instances(tlv, tlv_size, offset, resource, operation,false);
        if(M2MTLVDeserializer::None == error) {
            error = deserialize_resource_instances(tlv, tlv_size, offset, resource, operation,true);
        }
    }
    return error;
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialize_object_instances(const uint8_t *tlv,
                                                                           uint32_t tlv_size,
                                                                           uint32_t offset,
                                                                           M2MObject &object,
                                                                           M2MTLVDeserializer::Operation operation,
                                                                           bool update_value)
{
    tr_debug("M2MTLVDeserializer::deserialize_object_instances()");
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    TypeIdLength til(tlv, offset);
    til.deserialize();
    offset = til._offset;

    const M2MObjectInstanceList &list = object.instances();
    M2MObjectInstanceList::const_iterator it;
    it = list.begin();

    if (TYPE_OBJECT_INSTANCE == til._type) {
        for (; it!=list.end(); it++) {
            if((*it)->instance_id() == til._id) {
                error = deserialize_resources(tlv, tlv_size, offset, (**it),operation, update_value);
            }
        }
        offset += til._length;

        if(offset < tlv_size) {
            error = deserialize_object_instances(tlv, tlv_size, offset, object, operation, update_value);
        }
    }
    return error;
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialize_resources(const uint8_t *tlv,
                                                                    uint32_t tlv_size,
                                                                    uint32_t offset,
                                                                    M2MObjectInstance &object_instance,
                                                                    M2MTLVDeserializer::Operation operation,
                                                                    bool update_value)
{
    tr_debug("M2MTLVDeserializer::deserialize_resources()");
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    TypeIdLength til(tlv, offset);
    til.deserialize();
    offset = til._offset;

    const M2MResourceList &list = object_instance.resources();
    M2MResourceList::const_iterator it;
    it = list.begin();

    if (TYPE_RESOURCE == til._type || TYPE_RESOURCE_INSTANCE == til._type) {
        bool found = false;
        for (; it!=list.end(); it++) {
            if((*it)->name_id() == til._id){
                tr_debug("M2MTLVDeserializer::deserialize_resources() - Resource ID %d ", til._id);
                found = true;
                if(update_value) {
                    if(til._length > 0) {
                        tr_debug("M2MTLVDeserializer::deserialize_resources() - Update value");
                        set_resource_instance_value((*it), tlv+offset, til._length);
                    } else {
                        tr_debug("M2MTLVDeserializer::deserialize_resources() - Clear Value");
                        (*it)->clear_value();
                    }
                    break;
                } else if(0 == ((*it)->operation() & SN_GRS_PUT_ALLOWED)) {
                    tr_debug("M2MTLVDeserializer::deserialize_resources() - NOT_ALLOWED");
                    error = M2MTLVDeserializer::NotAllowed;
                    break;
                }
            }
        }
        if(!found) {
            if(M2MTLVDeserializer::Post == operation) {
                //Create a new Resource
                String id;
                id.append_int(til._id);
                M2MResource *resource = object_instance.create_dynamic_resource(id,"",M2MResourceInstance::INTEGER,true,false);
                if(resource) {
                    resource->set_operation(M2MBase::GET_PUT_POST_DELETE_ALLOWED);
                }
            } else if(M2MTLVDeserializer::Put == operation) {
                error = M2MTLVDeserializer::NotFound;
            }
        }
    } else if (TYPE_MULTIPLE_RESOURCE == til._type) {
        for (; it!=list.end(); it++) {
            if((*it)->supports_multiple_instances() &&
                    (*it)->name_id() == til._id) {
                error = deserialize_resource_instances(tlv, tlv_size, offset, (**it), object_instance, operation, update_value);
            }
        }

    } else {
        error = M2MTLVDeserializer::NotValid;
        return error;
    }
    offset += til._length;

    if(offset < tlv_size) {
        error = deserialize_resources(tlv, tlv_size, offset, object_instance, operation, update_value);
    }
    return error;
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialize_resource_instances(const uint8_t *tlv,
                                                                             uint32_t tlv_size,
                                                                             uint32_t offset,
                                                                             M2MResource &resource,
                                                                             M2MObjectInstance &object_instance,
                                                                             M2MTLVDeserializer::Operation operation,
                                                                             bool update_value)
{
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    TypeIdLength til(tlv, offset);
    til.deserialize();
    offset = til._offset;

    if (TYPE_MULTIPLE_RESOURCE == til._type || TYPE_RESOURCE_INSTANCE == til._type) {
        const M2MResourceInstanceList &list = resource.resource_instances();
        M2MResourceInstanceList::const_iterator it;
        it = list.begin();
        bool found = false;
        for (; it!=list.end(); it++) {
            if((*it)->instance_id() == til._id && TYPE_RESOURCE_INSTANCE == til._type) {
                found = true;
                if(update_value) {
                    if(til._length > 0) {
                        set_resource_instance_value((*it), tlv+offset, til._length);
                    } else {
                        (*it)->clear_value();
                    }
                    break;
                } else if(0 == ((*it)->operation() & SN_GRS_PUT_ALLOWED)) {
                    error = M2MTLVDeserializer::NotAllowed;
                    break;
                }
            }
        }
        if(!found) {
            if(M2MTLVDeserializer::Post == operation) {
                // Create a new Resource Instance
                M2MResourceInstance *res_instance = object_instance.create_dynamic_resource_instance(resource.name(),"",
                                                                                                 M2MResourceInstance::INTEGER,
                                                                                                 true,
                                                                                                 til._id);
                if(res_instance) {
                    res_instance->set_operation(M2MBase::GET_PUT_POST_DELETE_ALLOWED);
                }
            } else if(M2MTLVDeserializer::Put == operation) {
                error = M2MTLVDeserializer::NotFound;
            }
        }
    } else {
        error = M2MTLVDeserializer::NotValid;
        return error;
    }

    offset += til._length;

    if(offset < tlv_size) {
      error = deserialize_resource_instances(tlv, tlv_size, offset, resource, object_instance, operation, update_value);
    }
    return error;
}

M2MTLVDeserializer::Error M2MTLVDeserializer::deserialize_resource_instances(const uint8_t *tlv,
                                                                             uint32_t tlv_size,
                                                                             uint32_t offset,
                                                                             M2MResource &resource,
                                                                             M2MTLVDeserializer::Operation operation,
                                                                             bool update_value)
{
    M2MTLVDeserializer::Error error = M2MTLVDeserializer::None;
    TypeIdLength til(tlv, offset);
    til.deserialize();
    offset = til._offset;

    if (TYPE_RESOURCE_INSTANCE == til._type) {
        const M2MResourceInstanceList &list = resource.resource_instances();
        M2MResourceInstanceList::const_iterator it;
        it = list.begin();
        bool found = false;
        for (; it!=list.end(); it++) {
            if((*it)->instance_id() == til._id) {
                found = true;
                if(update_value) {
                    if(til._length > 0) {
                        set_resource_instance_value((*it),tlv+offset, til._length);
                    } else {
                        (*it)->clear_value();
                    }
                    break;
                } else if(0 == ((*it)->operation() & SN_GRS_PUT_ALLOWED)) {
                    error = M2MTLVDeserializer::NotAllowed;
                    break;
                }
            }
        }
        if(!found) {
            if(M2MTLVDeserializer::Post == operation) {
                error = M2MTLVDeserializer::NotAllowed;
            } else if(M2MTLVDeserializer::Put == operation) {
                error = M2MTLVDeserializer::NotFound;
            }
        }
    } else {
        error = M2MTLVDeserializer::NotValid;
        return error;
    }

    offset += til._length;

    if(offset < tlv_size) {
         error = deserialize_resource_instances(tlv, tlv_size, offset, resource, operation, update_value);
    }
    return error;
}

bool M2MTLVDeserializer::is_object_instance(const uint8_t *tlv, uint32_t offset)
{
    bool ret = false;
    if (tlv) {
        uint8_t value = tlv[offset];
        ret = (TYPE_OBJECT_INSTANCE == (value & TYPE_RESOURCE));
    }
    return ret;
}

uint16_t M2MTLVDeserializer::instance_id(const uint8_t *tlv)
{
    TypeIdLength til(tlv, 0);
    til.deserialize();
    uint16_t id = til._id;
    return id;
}

bool M2MTLVDeserializer::is_resource(const uint8_t *tlv, uint32_t offset)
{
    bool ret = false;
    if (tlv) {
        ret = (TYPE_RESOURCE == (tlv[offset] & TYPE_RESOURCE));
    }
    return ret;
}

bool M2MTLVDeserializer::is_multiple_resource(const uint8_t *tlv, uint32_t offset)
{
    bool ret = false;
    if (tlv) {
        ret = (TYPE_MULTIPLE_RESOURCE == (tlv[offset] & TYPE_RESOURCE));
    }
    return ret;
}

bool M2MTLVDeserializer::is_resource_instance(const uint8_t *tlv, uint32_t offset)
{
    bool ret = false;
    if (tlv) {
        ret = (TYPE_RESOURCE_INSTANCE == (tlv[offset] & TYPE_RESOURCE));
    }
    return ret;
}

void M2MTLVDeserializer::set_resource_instance_value(M2MResourceInstance *res, const uint8_t *tlv, const uint32_t size)
{
    int64_t value = 0;
    switch (res->resource_instance_type()) {
        case M2MResourceInstance::INTEGER:
        case M2MResourceInstance::BOOLEAN:
        case M2MResourceInstance::TIME:
            if(size < 9) { 
                value = String::convert_array_to_integer(tlv, size);
                res->set_value(value);
            }
            /* size > 8 then handle value same way like 
             * M2MResourceInstance::STRING */
            else { 
                res->set_value(tlv, size);
            }
            break;
        // Todo! implement conversion for other types as well
        case M2MResourceInstance::STRING:
        case M2MResourceInstance::FLOAT:
        case M2MResourceInstance::OPAQUE:
        case M2MResourceInstance::OBJLINK:
            res->set_value(tlv, size);
            break;
        default:
            break;
    }
}

TypeIdLength::TypeIdLength(const uint8_t *tlv, uint32_t offset)
: _tlv(tlv), _offset(offset), _type(tlv[offset] & 0xC0), _id(0), _length(0)
{
}

void TypeIdLength::deserialize()
{
    uint32_t idLength = _tlv[_offset] & ID16;
    uint32_t lengthType = _tlv[_offset] & LENGTH24;
    if (0 == lengthType) {
        _length = _tlv[_offset] & 0x07;
    }
    _offset++;

    deserialiseID(idLength);
    deserialiseLength(lengthType);
}

void TypeIdLength::deserialiseID(uint32_t idLength)
{
    _id = _tlv[_offset++] & 0xFF;
    if (ID16 == idLength) {
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
