/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_SECURITY_PIMPL_H
#define TEST_M2M_CONNECTION_SECURITY_PIMPL_H

#include "lwm2m-client-mbedtls/m2mconnectionsecuritypimpl.h"

class Test_M2MConnectionSecurityPimpl
{
public:
    Test_M2MConnectionSecurityPimpl();

    virtual ~Test_M2MConnectionSecurityPimpl();

    void test_constructor();

    void test_destructor();

    void test_reset();

    void test_init();

    void test_connect();

    void test_start_connecting_non_blocking();

    void test_continue_connecting();

    void test_send_message();

    void test_read();

    void test_timer_expired();
};


#endif // TEST_M2M_CONNECTION_SECURITY_PIMPL_H
