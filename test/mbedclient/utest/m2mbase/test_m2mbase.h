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
#ifndef TEST_M2M_BASE_H
#define TEST_M2M_BASE_H
#include "m2mbase.h"
#include "m2mobservationhandler.h"

class Handler : public M2MObservationHandler {

public:

    Handler() : visited(false) {}
    ~Handler(){}
    void observation_to_be_sent(M2MBase *, uint16_t, m2m::Vector<uint16_t>, bool){
        visited = true;
    }
    void send_delayed_response(M2MBase *){}
    void resource_to_be_deleted(M2MBase *){visited=true;}
    void remove_object(M2MBase *){visited = true;}
    void value_updated(M2MBase *,const String&){visited = true;}

    void clear() {visited = false;}
    bool visited;
};

class Test_M2MBase : M2MBase
{
public:
    Test_M2MBase(char* path, Handler *handler);

    virtual ~Test_M2MBase();

    void test_copy_constructor();

    void test_set_operation();

    void test_set_base_type();

    void test_set_interface_description();

    void test_uri_path();

    void test_set_resource_type();

    void test_set_coap_content_type();

    void test_set_instance_id();

    void test_set_observable();

    void test_add_observation_level();

    void test_remove_observation_level();

    void test_set_under_observation();

    void test_set_observation_token();

    void test_observation_level();

    void test_get_observation_token();

    void test_mode();

    void test_observation_number();

    void test_name();

    void test_name_id();

    void test_coap_content_type();

    void test_base_type();

    void test_handle_observation_attribute();

    void test_observation_to_be_sent();

    void test_handle_get_request();

    void test_handle_put_request();

    void test_handle_post_request();

    void test_memory_alloc();

    void test_memory_free();

    void test_report_handler();

    void test_observation_handler();

    void test_id_number();

    void test_set_register_uri();

    void test_set_observation_number();

    void test_set_max_age();

    void test_is_under_observation();

    void test_value_updated_function();

    void test_build_path();

    void test_set_observation_handler();

    void test_resource_type();

    void test_resource_name_length();

    void test_get_nsdl_resource();

    void test_create_path();

    void test_create_report_handler();

    void test_validate_string_length();

    void test_is_integer();

    void test_alloc_copy();

    void test_alloc_string_copy();

    void test_ctor();

    Handler *obsHandler;
};


#endif // TEST_M2M_BASE_H
