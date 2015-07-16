/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_SECURITY_IMPL_H
#define TEST_M2M_CONNECTION_SECURITY_IMPL_H

#include "lwm2m-client/m2mconnectionsecurity.h"

class Test_M2MConnectionSecurityImpl
{
public:
    Test_M2MConnectionSecurityImpl();

    virtual ~Test_M2MConnectionSecurityImpl();

    void test_constructor();

    void test_destructor();

    void test_reset();

    void test_init();

    void test_connect();

    void test_start_connecting_non_blocking();

    void test_continue_connecting();

    void test_send_message();

    void test_read();
};


#endif // TEST_M2M_CONNECTION_SECURITY_IMPL_H
