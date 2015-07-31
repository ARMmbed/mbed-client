/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_RESOURCE_INSTANCE_H
#define TEST_M2M_RESOURCE_INSTANCE_H

#include "m2mresourceinstance.h"

class Test_M2MResourceInstance
{
public:
    Test_M2MResourceInstance();

    void test_copy_constructor();

    virtual ~Test_M2MResourceInstance();

    void test_base_type();

    void test_resource_instance_type();

    void test_handle_observation_attribute();

    void test_set_execute_function();

    void test_execute();

    void test_set_value();

    void test_get_value();

    void test_value();

    void test_value_length();

    void test_handle_get_request();

    void test_handle_put_request();

    void test_handle_post_request();

    M2MResourceInstance* resource_instance;
};

#endif // TEST_M2M_RESOURCE_INSTANCE_H
