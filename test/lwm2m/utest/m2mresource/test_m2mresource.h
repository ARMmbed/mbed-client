/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_RESOURCE_H
#define TEST_M2M_RESOURCE_H

#include "m2mresource.h"

class Test_M2MResource
{
public:
    Test_M2MResource();

    void test_copy_constructor();

    virtual ~Test_M2MResource();

    void test_base_type();

    void test_muliptle_instances();

    void test_handle_observation_attribute();

    void test_add_resource_instance();

    void test_remove_resource_instance();

    void test_resource_instance();

    void test_resource_instances();

    void test_resource_instance_count();

    M2MResource* resource;
};

#endif // TEST_M2M_RESOURCE_H
