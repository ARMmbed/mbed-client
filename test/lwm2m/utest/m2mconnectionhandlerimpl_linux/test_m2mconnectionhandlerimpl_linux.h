/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_HANDLER_IMPL_LINUX_H
#define TEST_M2M_CONNECTION_HANDLER_IMPL_LINUX_H

#include "lwm2m-client/m2mconnectionhandler.h"

class M2MConnection_TestObserver;

class Test_M2MConnectionHandler_linux
{
public:
    Test_M2MConnectionHandler_linux();

    virtual ~Test_M2MConnectionHandler_linux();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_start_listening_for_data();

//    void test_data_receive();

    void test_stop_listening();

    void test_sendToSocket();

    void test_receiveFromSocket();


    M2MConnectionHandler* handler;
    M2MConnection_TestObserver *observer;
};

#endif // TEST_M2M_CONNECTION_HANDLER_IMPL_LINUX_H
