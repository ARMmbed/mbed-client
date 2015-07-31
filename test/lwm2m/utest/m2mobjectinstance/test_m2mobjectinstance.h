/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_OBJECT_INSTANCE_H
#define TEST_M2M_OBJECT_INSTANCE_H

#include "m2mobjectinstance.h"

class Test_M2MObjectInstance
{
public:
    Test_M2MObjectInstance();

    void test_copy_constructor();

    virtual ~Test_M2MObjectInstance();

    void test_create_static_resource();

    void test_create_static_resource_instance();

    void test_create_dynamic_resource();

    void test_create_dynamic_resource_instance();

    void test_remove_resource();

    void test_resource();

    void test_resources();

    void test_resource_count();

    void test_total_resource_count();

    void test_base_type();

    void test_handle_observation_attribute();

    void test_handle_get_request();

    void test_handle_put_request();

    void test_handle_post_request();

    M2MObjectInstance* object;
};

#endif // TEST_M2M_OBJECT_INSTANCE_H
