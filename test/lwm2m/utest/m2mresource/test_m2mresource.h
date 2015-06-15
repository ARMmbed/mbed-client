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

    void test_set_execute_function();

    void test_execute();

    M2MResource* resource;
};

#endif // TEST_M2M_RESOURCE_H
