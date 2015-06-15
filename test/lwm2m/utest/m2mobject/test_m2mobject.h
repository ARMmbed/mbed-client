/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_OBJECT_H
#define TEST_M2M_OBJECT_H

#include "m2mobject.h"

class Test_M2MObject
{
public:
    Test_M2MObject();
    virtual ~Test_M2MObject();

    void test_copy_constructor();

    void test_create_object_instance();

    void test_remove_object_instance();

    void test_object_instance();

    void test_instances();

    void test_instance_count();

    void test_base_type();

    void test_handle_observation_attribute();

    M2MObject* object;
};

#endif // TEST_M2M_OBJECT_H
