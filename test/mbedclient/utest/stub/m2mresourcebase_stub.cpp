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
#include "m2mresourcebase_stub.h"


uint32_t m2mresourcebase_stub::int_value;
bool m2mresourcebase_stub::bool_value;
String *m2mresourcebase_stub::string_value;
M2MResourceBase::ResourceType m2mresourcebase_stub::resource_type;
sn_coap_hdr_s *m2mresourcebase_stub::header;
uint8_t* m2mresourcebase_stub::value;


void m2mresourcebase_stub::clear()
{
    int_value = 0;
    bool_value = false;
    resource_type = M2MResourceBase::STRING;
    header = NULL;
    value = NULL;
    string_value = NULL;
}

M2MResourceBase::M2MResourceBase(
                                 const String &res_name,
                                 M2MBase::Mode resource_mode,
                                 const String &resource_type,
                                 M2MBase::DataType type,
                                 char* path,
                                 bool external_blockwise_store,
                                 bool multiple_instance)
: M2MBase(res_name,
          resource_mode,
#ifndef DISABLE_RESOURCE_TYPE
          resource_type,
#endif
          path,
          external_blockwise_store,
          multiple_instance,
          type)
#ifndef DISABLE_BLOCK_MESSAGE
 ,_block_message_data(NULL)
#endif
{
}

M2MResourceBase::M2MResourceBase(
                                 const String &res_name,
                                 M2MBase::Mode resource_mode,
                                 const String &resource_type,
                                 M2MBase::DataType type,
                                 const uint8_t *value,
                                 const uint8_t value_length,
                                 char* path,
                                 bool external_blockwise_store,
                                 bool multiple_instance)
: M2MBase(res_name,
          resource_mode,
#ifndef DISABLE_RESOURCE_TYPE
          resource_type,
#endif
          path,
          external_blockwise_store,
          multiple_instance,
          type)
#ifndef DISABLE_BLOCK_MESSAGE
 ,_block_message_data(NULL)
#endif
{
    set_base_type(M2MBase::ResourceInstance);
}

M2MResourceBase::M2MResourceBase(
                                         const lwm2m_parameters_s* s,
                                         M2MBase::DataType /*type*/)
: M2MBase(s)
#ifndef DISABLE_BLOCK_MESSAGE
  ,_block_message_data(NULL)
#endif
{
}

M2MResourceBase::~M2MResourceBase()
{
}

bool M2MResourceBase::set_execute_function(execute_callback)
{
}

bool M2MResourceBase::set_value(const uint8_t *,
                                    const uint32_t )
{
    return m2mresourcebase_stub::bool_value;
}

bool M2MResourceBase::set_value(int64_t value)
{
    return m2mresourcebase_stub::bool_value;
}


void M2MResourceBase::clear_value()
{
    if (m2mresourcebase_stub::value) {
        free(m2mresourcebase_stub::value);
        m2mresourcebase_stub::value = NULL;
        m2mresourcebase_stub::int_value = 0;
    }
}

void M2MResourceBase::execute(void *)
{
}

void M2MResourceBase::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    value = (uint8_t *)malloc(m2mresourcebase_stub::int_value);
    if(value) {
        value_length = m2mresourcebase_stub::int_value;
        memcpy((uint8_t *)value, (uint8_t *)m2mresourcebase_stub::value, value_length);
    }
}

int64_t M2MResourceBase::get_value_int() const
{
    // Note: this is a copy-paste from the original version, as the tests
    // set only m2mresourcebase_stub::value.

    int value_int = 0;
    uint8_t* buffer = m2mresourcebase_stub::value;

    if(buffer) {
        value_int = atoi((const char*)buffer);
    }
    return value_int;
}

String M2MResourceBase::get_value_string() const
{
    // XXX: do a better constructor to avoid pointless malloc
    String value;
    if (m2mresourcebase_stub::value) {
        value.append_raw((char*)m2mresourcebase_stub::value, m2mresourcebase_stub::int_value);
    }

    return value;
}

uint8_t* M2MResourceBase::value() const
{
    return m2mresourcebase_stub::value;
}

uint32_t M2MResourceBase::value_length() const
{
    return m2mresourcebase_stub::int_value;
}

sn_coap_hdr_s* M2MResourceBase::handle_get_request(nsdl_s *,
                                               sn_coap_hdr_s *,
                                               M2MObservationHandler *)
{
    return m2mresourcebase_stub::header;
}

sn_coap_hdr_s* M2MResourceBase::handle_put_request(nsdl_s *,
                                               sn_coap_hdr_s *,
                                               M2MObservationHandler *,
                                               bool &)
{
    return m2mresourcebase_stub::header;
}

uint16_t M2MResourceBase::object_instance_id() const
{
    return m2mresourcebase_stub::int_value;
}

M2MResourceBase::ResourceType M2MResourceBase::resource_instance_type() const
{
}

bool M2MResourceBase::set_execute_function(execute_callback_2 callback)
{
}

M2MBlockMessage* M2MResourceBase::block_message() const
{

}

bool M2MResourceBase::set_incoming_block_message_callback(incoming_block_message_callback callback)
{
}

bool M2MResourceBase::set_outgoing_block_message_callback(outgoing_block_message_callback callback)
{

}

bool M2MResourceBase::set_notification_sent_callback(notification_sent_callback callback)
{
}

bool M2MResourceBase::set_notification_sent_callback(notification_sent_callback_2 callback)
{
}

void M2MResourceBase::notification_sent()
{
}

const char* M2MResourceBase::object_name() const
{

}
