/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_HANDLER_IMPL_MBED_H
#define TEST_M2M_CONNECTION_HANDLER_IMPL_MBED_H

#include "lwm2m-client-mbed/m2mconnectionhandlerimpl.h"

class TestObserver;

class Test_M2MConnectionHandlerImpl_mbed
{
public:
    Test_M2MConnectionHandlerImpl_mbed();

    virtual ~Test_M2MConnectionHandlerImpl_mbed();

    void test_constructor();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_listen_for_data();

    void test_close_connection();

    //Special case: test privates also
    void test_send_handler();
    void test_receive_handler();
    void test_dns_handler();
    void test_error_handler();
    void test_get_network_stack();

    M2MConnectionHandlerImpl* handler;
    TestObserver *observer;
};

#endif // TEST_M2M_CONNECTION_HANDLER_IMPL_MBED_H
