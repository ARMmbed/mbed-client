/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_SERVER_H
#define TEST_M2M_SERVER_H

#include "m2mserver.h"

class Test_M2MServer
{
public:
    Test_M2MServer();
    virtual ~Test_M2MServer();

    void test_create_resource();

    void test_create_resource_int();

    void test_delete_resource();

    void test_set_resource_value_int();

    void test_set_resource_value_string();

    void test_resource_value_int();

    void test_resource_value_string();

    void test_is_resource_present();

    void test_total_resource_count();

    M2MServer* server;
};

#endif // TEST_M2M_SERVER_H

