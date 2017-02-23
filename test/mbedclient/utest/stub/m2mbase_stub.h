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
#ifndef M2M_BASE_STUB_H
#define M2M_BASE_STUB_H

#include "m2mbase.h"
#include "nsdlaccesshelper_stub.h"
static sn_nsdl_static_resource_parameters_s params_static = {
    (char*)"",      // resource_type_ptr
    (char*)"",                     // interface_description_ptr
    (uint8_t*)"",    // path
    (uint8_t*)"value",           // resource
    0,                      // pathlen
    5,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

static sn_nsdl_dynamic_resource_parameters_s params_dynamic = {
    __nsdl_c_callback,
    &params_static,
    {NULL, NULL},                     // link
    0, // coap_content_type
    M2MBase::PUT_ALLOWED,   // access
    0,                      // registered
    false,                  // publish_uri
    false,                  // free_on_delete
    true                    // observable
};

const static M2MBase::lwm2m_parameters params = {
    0, // max_age
    1, // instance_id
    0, // name_id
    (char*)"", // name
    &params_dynamic,
    M2MBase::Resource, // base_type
    false // free_on_delete
};

//some internal test related stuff
namespace m2mbase_stub
{
    extern uint8_t uint8_value;
    extern uint16_t uint16_value;
    extern uint32_t uint32_value;
    extern uint16_t int_value;
    extern int32_t name_id_value;
    extern const char *string_value;
    extern const char *object_instance_name;
    extern const char *resource_name;
    extern const char *resource_name_inst;
    extern bool bool_value;
    extern M2MBase::BaseType base_type;
    extern M2MBase::Mode mode_value;
    extern M2MBase::Operation operation;
    extern void *void_value;
    extern M2MObservationHandler *observe;
    extern M2MReportHandler *report;
    extern M2MBase::Observation observation_level_value;
    extern bool is_value_updated_function_set;
    void clear();
    extern uint8_t* object_token;
    extern uint32_t object_token_len;
    extern uint8_t* resource_token;
    extern uint32_t resource_token_len;
    extern uint8_t* object_inst_token;
    extern uint32_t object_inst_token_len;
    extern sn_nsdl_dynamic_resource_parameters_s *nsdl_resource;
    extern bool find_resource;
    extern int32_t ret_counter;
}

#endif // M2M_BASE_STUB_H

