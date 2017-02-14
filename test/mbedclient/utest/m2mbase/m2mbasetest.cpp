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
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mbase.h"

TEST_GROUP(M2MBase)
{
    Test_M2MBase* m2m_base;
    Handler *obsHandler;
    void setup() {
        // Path ownership moved to m2mbase
        char* path = (char*)malloc(5);
        strcpy(path, "test");

        obsHandler = new Handler();
        m2m_base = new Test_M2MBase(path, obsHandler);
    }

    void teardown() {
        delete m2m_base;
        delete obsHandler;
    }
};

TEST(M2MBase, Create)
{
    CHECK(m2m_base != NULL);
}

TEST(M2MBase, set_operation)
{
    m2m_base->test_set_operation();
}

TEST(M2MBase, test_set_base_type)
{
    m2m_base->test_set_base_type();
}

TEST(M2MBase, set_interface_description)
{
    m2m_base->test_set_interface_description();
}

TEST(M2MBase, uri_path)
{
    m2m_base->test_uri_path();
}

TEST(M2MBase, set_resource_type)
{
    m2m_base->test_set_resource_type();
}

TEST(M2MBase, set_coap_content_type)
{
    m2m_base->test_set_coap_content_type();
}

TEST(M2MBase, set_instance_id)
{
    m2m_base->test_set_instance_id();
}

TEST(M2MBase, observation_number)
{
    m2m_base->test_observation_number();
}

TEST(M2MBase, mode)
{
    m2m_base->test_mode();
}

TEST(M2MBase, observation_token)
{
    m2m_base->test_get_observation_token();
}

TEST(M2MBase, observation_level)
{
    m2m_base->test_observation_level();
}

TEST(M2MBase, set_observation_token)
{
    m2m_base->test_set_observation_token();
}

TEST(M2MBase, set_under_observation)
{
    m2m_base->test_set_under_observation();
}

TEST(M2MBase, set_observable)
{
    m2m_base->test_set_observable();
}

TEST(M2MBase, add_observation_level)
{
    m2m_base->test_add_observation_level();
}

TEST(M2MBase, remove_observation_level)
{
    m2m_base->test_remove_observation_level();
}

TEST(M2MBase, name)
{
    m2m_base->test_name();
}

TEST(M2MBase, name_id)
{
    m2m_base->test_name_id();
}

TEST(M2MBase, test_handle_observation_attribute)
{
    m2m_base->test_handle_observation_attribute();
}

TEST(M2MBase, test_observation_to_be_sent)
{
    m2m_base->test_observation_to_be_sent();
}

TEST(M2MBase, test_handle_get_request)
{
    m2m_base->test_handle_get_request();
}

TEST(M2MBase, test_handle_put_request)
{
    m2m_base->test_handle_put_request();
}

TEST(M2MBase, test_handle_post_request)
{
    m2m_base->test_handle_post_request();
}

TEST(M2MBase, test_memory_alloc)
{
    m2m_base->test_memory_alloc();
}

TEST(M2MBase, test_memory_free)
{
    m2m_base->test_memory_free();
}

TEST(M2MBase, test_report_handler)
{
    m2m_base->test_report_handler();
}

TEST(M2MBase, test_observation_handler)
{
    m2m_base->test_observation_handler();
}

TEST(M2MBase, test_id_number)
{
    m2m_base->test_id_number();
}

TEST(M2MBase, test_set_register_uri)
{
    m2m_base->test_set_register_uri();
}

TEST(M2MBase, test_set_observation_number)
{
    m2m_base->test_set_observation_number();
}

TEST(M2MBase, test_set_max_age)
{
    m2m_base->test_set_max_age();
}

TEST(M2MBase, test_is_under_observation)
{
    m2m_base->test_is_under_observation();
}

TEST(M2MBase, test_value_updated_function)
{
    m2m_base->test_value_updated_function();
}

TEST(M2MBase, test_build_path)
{
    m2m_base->test_build_path();
}

TEST(M2MBase, test_set_observation_handler)
{
    m2m_base->test_set_observation_handler();
}

TEST(M2MBase, test_resource_type)
{
    m2m_base->test_resource_type();
}

TEST(M2MBase, test_resource_name_length)
{
    m2m_base->test_resource_name_length();
}

TEST(M2MBase, test_get_nsdl_resource)
{
    m2m_base->test_get_nsdl_resource();
}

TEST(M2MBase, test_create_path)
{
    m2m_base->test_create_path();
}

TEST(M2MBase, test_create_report_handler)
{
    m2m_base->test_create_report_handler();
}

TEST(M2MBase, test_validate_string_length)
{
    m2m_base->test_validate_string_length();
}

TEST(M2MBase, test_is_integer)
{
    m2m_base->test_is_integer();
}

TEST(M2MBase, test_alloc_string_copy)
{
    m2m_base->test_alloc_string_copy();
}

TEST(M2MBase, test_alloc_copy)
{
    m2m_base->test_alloc_copy();
}

TEST(M2MBase, test_ctor)
{
    m2m_base->test_ctor();
}
