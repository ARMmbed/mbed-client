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
M2MBase::BaseType m2mresourceinstance_stub::base_type;


void m2mresourceinstance_stub::clear()
{
    int_value = 0;
    bool_value = false;
    resource_type = M2MResourceInstance::STRING;
    header = NULL;
    value = NULL;
    base_type = M2MBase::ResourceInstance;
    string_value = NULL;
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         const uint16_t object_instance_id,
                                         char* path,
                                         bool external_blockwise_store)
: M2MBase(res_name,
          M2MBase::Dynamic,
          resource_type,
          path,
          external_blockwise_store),
 _parent_resource(parent),
 _value(NULL),
 _value_length(0),
 _block_message_data(NULL),
 _execute_callback(NULL),
 _resource_callback(NULL),
 _execute_function_pointer(NULL),
 _notification_sent_function_pointer(NULL),
 _notification_sent_callback(NULL),
 _object_instance_id(object_instance_id),
 _resource_type(type)
{

}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         const uint16_t object_instance_id,
                                         char* path,
                                         bool external_blockwise_store)
: M2MBase(res_name,
          M2MBase::Static,
          resource_type,
          path,
          external_blockwise_store),
 _parent_resource(parent),
 _value(NULL),
 _value_length(0),
 _block_message_data(NULL),
 _execute_callback(NULL),
 _resource_callback(NULL),
 _execute_function_pointer(NULL),
 _notification_sent_function_pointer(NULL),
 _notification_sent_callback(NULL),
 _object_instance_id(object_instance_id),
  _resource_type(type)
{
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const lwm2m_parameters_s* s,
                                         M2MResourceInstance::ResourceType type,
                                         const uint16_t object_instance_id)
: M2MBase(s),
  _parent_resource(parent),
  _value(NULL),
  _value_length(0),
  _block_message_data(NULL),
  _execute_callback(NULL),
  _resource_callback(NULL),
  _execute_function_pointer(NULL),
  _notification_sent_function_pointer(NULL),
  _object_instance_id(object_instance_id),
  _resource_type(type)
{
}

M2MResourceInstance::~M2MResourceInstance()
{
}

M2MBase::BaseType M2MResourceInstance::base_type() const
{
    return m2mresourceinstance_stub::base_type;
}

M2MResourceInstance::ResourceType M2MResourceInstance::resource_instance_type() const
{
    return m2mresourceinstance_stub::resource_type;
}

bool M2MResourceInstance::handle_observation_attribute(const char *)
{
    return m2mresourceinstance_stub::bool_value;
}

void M2MResourceInstance::set_execute_function(execute_callback)
{
}

bool M2MResourceInstance::set_value(const uint8_t *,
                                    const uint32_t )
{
    return m2mresourceinstance_stub::bool_value;
}

bool M2MResourceInstance::set_value(int64_t value)
{
    return m2mresourceinstance_stub::bool_value;
}


void M2MResourceInstance::clear_value()
{
    if (m2mresourceinstance_stub::value) {
        free(m2mresourceinstance_stub::value);
        m2mresourceinstance_stub::value = NULL;
        m2mresourceinstance_stub::int_value = 0;
    }
}

void M2MResourceInstance::execute(void *)
{
}

void M2MResourceInstance::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    value = (uint8_t *)malloc(m2mresourceinstance_stub::int_value);
    if(value) {
        value_length = m2mresourceinstance_stub::int_value;
        memcpy((uint8_t *)value, (uint8_t *)m2mresourceinstance_stub::value, value_length);
    }
}

int M2MResourceInstance::get_value_int()
{
    // Note: this is a copy-paste from the original version, as the tests
    // set only m2mresourceinstance_stub::value.

    int value_int = 0;
    // Get the value and convert it into integer. This is not the most
    // efficient way, as it takes pointless heap copy to get the zero termination.
    uint8_t* buffer = NULL;
    uint32_t length;
    get_value(buffer,length);
    if(buffer) {
        value_int = atoi((const char*)buffer);
        free(buffer);
    }
    return value_int;
}

String M2MResourceInstance::get_value_string() const
{
    // XXX: do a better constructor to avoid pointless malloc
    String value;
    if (m2mresourceinstance_stub::value) {
        value.append_raw((char*)m2mresourceinstance_stub::value, m2mresourceinstance_stub::int_value);
    }

    return value;
}

uint8_t* M2MResourceInstance::value() const
{
    return m2mresourceinstance_stub::value;
}

uint32_t M2MResourceInstance::value_length() const
{
    return m2mresourceinstance_stub::int_value;
}

sn_coap_hdr_s* M2MResourceInstance::handle_get_request(nsdl_s *,
                                               sn_coap_hdr_s *,
                                               M2MObservationHandler *)
{
    return m2mresourceinstance_stub::header;
}

sn_coap_hdr_s* M2MResourceInstance::handle_put_request(nsdl_s *,
                                               sn_coap_hdr_s *,
                                               M2MObservationHandler *,
                                               bool &)
{
    return m2mresourceinstance_stub::header;
}

void M2MResourceInstance::set_resource_observer(M2MResourceCallback *callback)
{

}

uint16_t M2MResourceInstance::object_instance_id() const
{
    return m2mresourceinstance_stub::int_value;
}

void M2MResourceInstance::set_execute_function(execute_callback_2 callback)
{
}

M2MBlockMessage* M2MResourceInstance::block_message() const
{

}

void M2MResourceInstance::set_incoming_block_message_callback(incoming_block_message_callback callback)
{
}

void M2MResourceInstance::set_outgoing_block_message_callback(outgoing_block_message_callback callback)
{

}

void M2MResourceInstance::set_notification_sent_callback(notification_sent_callback callback)
{
}

void M2MResourceInstance::set_notification_sent_callback(notification_sent_callback_2 callback)
{
}

void M2MResourceInstance::notification_sent()
{
}

const char* M2MResourceInstance::object_name() const
{

}
