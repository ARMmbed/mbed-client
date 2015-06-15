/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_SECURITY_H
#define TEST_M2M_SECURITY_H

#include "m2msecurity.h"

class Test_M2MSecurity
{
public:
    Test_M2MSecurity();
    virtual ~Test_M2MSecurity();

    void test_create_resource_int();

    void test_delete_resource();

    void test_set_resource_value_int();

    void test_set_resource_value_string();

    void test_set_resource_value_buffer();

    void test_resource_value_int();

    void test_resource_value_string();

    void test_resource_value_buffer();

    void test_is_resource_present();

    void test_total_resource_count();

    void test_m2m_server_constructor();

    void test_server_type();

    M2MSecurity* security;
};

#endif // TEST_M2M_SECURITY_H
