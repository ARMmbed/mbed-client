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
#include "m2mresourceinstance_stub.h"

uint32_t m2mresourceinstance_stub::int_value;
bool m2mresourceinstance_stub::bool_value;
String *m2mresourceinstance_stub::string_value;
M2MResourceInstance::ResourceType m2mresourceinstance_stub::resource_type;
sn_coap_hdr_s *m2mresourceinstance_stub::header;
uint8_t* m2mresourceinstance_stub::value;


void m2mresourceinstance_stub::clear()
{
    int_value = 0;
    bool_value = false;
    resource_type = M2MResourceInstance::STRING;
    header = NULL;
    value = NULL;
    string_value = NULL;
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         M2MBase::Mode resource_mode,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MResourceBase(res_name,
            resource_mode,
          resource_type,
          type,
          path,
          external_blockwise_store,
          multiple_instance),
 _parent_resource(parent)
{
    set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         M2MBase::Mode resource_mode,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MResourceBase(res_name,
            resource_mode,
          resource_type,
          type,
          value,
          value_length,
          path,
          external_blockwise_store,
          multiple_instance),
 _parent_resource(parent)
{
    set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const lwm2m_parameters_s* s,
                                         M2MBase::DataType type)
: M2MResourceBase(s, type),
  _parent_resource(parent)
{
    set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::~M2MResourceInstance()
{
}

M2MObservationHandler* M2MResourceInstance::observation_handler() const
{
    return NULL;
}

void M2MResourceInstance::set_observation_handler(M2MObservationHandler *handler)
{
}

bool M2MResourceInstance::handle_observation_attribute(const char *)
{
    return m2mresourceinstance_stub::bool_value;
}

uint16_t M2MResourceInstance::object_instance_id() const
{
    return m2mresourceinstance_stub::int_value;
}

M2MResource& M2MResourceInstance::get_parent_resource() const
{
    return _parent_resource;
}

const char* M2MResourceInstance::object_name() const
{

}

