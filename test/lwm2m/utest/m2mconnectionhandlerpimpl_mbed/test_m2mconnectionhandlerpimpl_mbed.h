/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_HANDLER_PIMPL_MBED_H
#define TEST_M2M_CONNECTION_HANDLER_PIMPL_MBED_H

#include "lwm2m-client-mbed/m2mconnectionhandlerpimpl.h"

class TestObserver;

class Test_M2MConnectionHandlerPimpl_mbed
{
public:
    Test_M2MConnectionHandlerPimpl_mbed();

    virtual ~Test_M2MConnectionHandlerPimpl_mbed();

    void test_constructor();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_start_listening_for_data();

    void test_stop_listening();

    void test_sendToSocket();

    void test_receiveFromSocket();

    //Special case: test privates also
    void test_send_handler();
    void test_receive_handler();
    void test_receive_handshake_handler();
    void test_dns_handler();
    void test_error_handler();

    M2MConnectionHandlerPimpl* handler;
    TestObserver *observer;
};

#endif // TEST_M2M_CONNECTION_HANDLER_PIMPL_MBED_H
