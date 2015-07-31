/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_BASE_H
#define TEST_M2M_BASE_H

#include "m2mbase.h"

class Test_M2MBase : M2MBase
{
public:    
    Test_M2MBase();

    virtual ~Test_M2MBase();

    void test_copy_constructor();

    void test_assignment_operator();

    void test_set_operation();

    void test_set_base_type();

    void test_set_interface_description();

    void test_set_resource_type();

    void test_set_coap_content_type();

    void test_set_instance_id();

    void test_set_observable();

    void test_set_under_observation();

    void test_set_observation_token();

    void test_is_observable();

    void test_get_observation_token();

    void test_mode();

    void test_observation_number();

    void test_operation();

    void test_name();

    void test_instance_id();

    void test_interface_description();

    void test_resource_type();

    void test_coap_content_type();

    void test_base_type();

//    void test_set_value();

    void test_set_observation_number();

//    void test_get_value();

    void test_handle_observation_attribute();

    void test_observation_to_be_sent();

    void test_remove_resource_from_coap();

    void test_remove_object_from_coap();

};


#endif // TEST_M2M_BASE_H
