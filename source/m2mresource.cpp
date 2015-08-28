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
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mresource.h"
#include "include/nsdllinker.h"
#include "ns_trace.h"

M2MResource& M2MResource::operator=(const M2MResource& other)
{
    if (this != &other) { // protect against invalid self-assignment
        _has_multiple_instances = other._has_multiple_instances;
        if(!other._resource_instance_list.empty()){
            M2MResourceInstance* ins = NULL;
            M2MResourceInstanceList::const_iterator it;
            it = other._resource_instance_list.begin();
            for (; it!=other._resource_instance_list.end(); it++ ) {
                ins = *it;
                _resource_instance_list.push_back(new M2MResourceInstance(*ins));
            }
        }
    }
    return *this;
}

M2MResource::M2MResource(const M2MResource& other)
: M2MResourceInstance(other)
{
    this->operator=(other);
}

M2MResource::M2MResource(M2MObjectInstanceCallback &object_instance_callback,
                         const String &resource_name,
                         const String &resource_type,
                         M2MResourceInstance::ResourceType type,
                         const uint8_t *value,
                         const uint8_t value_length,
                         bool multiple_instance)
: M2MResourceInstance(resource_name, resource_type, type, value, value_length,
                      object_instance_callback),
  _has_multiple_instances(multiple_instance)
{
    M2MBase::set_base_type(M2MBase::Resource);
    M2MBase::set_operation(M2MBase::GET_ALLOWED);
    M2MBase::set_observable(false);
}

M2MResource::M2MResource(M2MObjectInstanceCallback &object_instance_callback,
                         const String &resource_name,
                         const String &resource_type,
                         M2MResourceInstance::ResourceType type,
                         bool observable,
                         bool multiple_instance)
: M2MResourceInstance(resource_name, resource_type, type,
                      object_instance_callback),
  _has_multiple_instances(multiple_instance)
{
    M2MBase::set_base_type(M2MBase::Resource);
    M2MBase::set_operation(M2MBase::GET_PUT_ALLOWED);
    M2MBase::set_observable(observable);
}

M2MResource::~M2MResource()
{
    if(!_resource_instance_list.empty()) {
        M2MResourceInstance* res = NULL;
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for (; it!=_resource_instance_list.end(); it++ ) {
            //Free allocated memory for resources.
            res = *it;
            delete res;
            res = NULL;
        }
        _resource_instance_list.clear();
    }
}

bool M2MResource::supports_multiple_instances() const
{
    return _has_multiple_instances;
}

bool M2MResource::remove_resource_instance(uint16_t inst_id)
{
    tr_debug("M2MResource::remove_resource(inst_id %d)", inst_id);
    bool success = false;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstance* res = NULL;
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        int pos = 0;
        for ( ; it != _resource_instance_list.end(); it++, pos++ ) {
            if(((*it)->instance_id() == inst_id)) {
                // Resource found and deleted.
                res = *it;
                delete res;
                res = NULL;
                _resource_instance_list.erase(pos);
                success = true;
                break;
            }
        }
    }
    return success;
}

M2MResourceInstance* M2MResource::resource_instance(uint16_t inst_id) const
{
    tr_debug("M2MResource::resource(resource_name inst_id %d)", inst_id);
    M2MResourceInstance *res = NULL;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for ( ; it != _resource_instance_list.end(); it++ ) {
            if(((*it)->instance_id() == inst_id)) {
                // Resource found.
                res = *it;
                break;
            }
        }
    }
    return res;
}

const M2MResourceInstanceList& M2MResource::resource_instances() const
{
    return _resource_instance_list;
}

uint16_t M2MResource::resource_instance_count() const
{
    return (uint16_t)_resource_instance_list.size();
}

bool M2MResource::handle_observation_attribute(char *&query)
{
    bool success = false;
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator it;
        it = _resource_instance_list.begin();
        for ( ; it != _resource_instance_list.end(); it++ ) {
            tr_debug("M2MResource::handle_observation_attribute()");
            success = (*it)->handle_observation_attribute(query);
        }
    } else {
        tr_debug("M2MResource::handle_observation_attribute() - else");
        // Apply write attributes only if resource is numerical
        M2MResourceInstance::ResourceType type = M2MResourceInstance::resource_instance_type();
        if (type == M2MResourceInstance::INTEGER ||
            type == M2MResourceInstance::FLOAT)
            success = M2MBase::handle_observation_attribute(query);
    }
    return success;
}

void M2MResource::add_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::add_observation_level(observation_level);
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator inst;
        inst = _resource_instance_list.begin();
        for ( ; inst != _resource_instance_list.end(); inst++ ) {
            (*inst)->add_observation_level(observation_level);
        }
    }
}

void M2MResource::remove_observation_level(M2MBase::Observation observation_level)
{
    M2MBase::remove_observation_level(observation_level);
    if(!_resource_instance_list.empty()) {
        M2MResourceInstanceList::const_iterator inst;
        inst = _resource_instance_list.begin();
        for ( ; inst != _resource_instance_list.end(); inst++ ) {
            (*inst)->remove_observation_level(observation_level);
        }
    }
}

void M2MResource::add_resource_instance(M2MResourceInstance *res)
{
    tr_debug("M2MResource::add_resource_instance()");
    if(res) {
        _resource_instance_list.push_back(res);
    }
}
