/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_DEVICE_H
#define TEST_M2M_DEVICE_H

//#include "test_defines.h"

#include <m2mdevice.h>

class Test_M2MDevice
{
public:
    Test_M2MDevice();
    virtual ~Test_M2MDevice();

    void test_create_resource_string();

    void test_create_resource_int();

    void test_create_resource_no_param();

    void test_delete_resource();

    void test_set_resource_value_int();

    void test_set_resource_value_string();

    void test_resource_value_int();

    void test_resource_value_string();

    void test_is_resource_present();

    void test_per_resource_count();

    void test_total_resource_count();

    M2MDevice* device;
};

#endif // TEST_M2M_DEVICE_H
