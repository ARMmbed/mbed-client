/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_HANDLER_IMPL_LINUX_H
#define TEST_M2M_CONNECTION_HANDLER_IMPL_LINUX_H

#include "lwm2m-client-linux/m2mconnectionhandlerimpl.h"

class M2MConnection_TestObserver;

class Test_M2MConnectionHandlerImpl_linux
{
public:
    Test_M2MConnectionHandlerImpl_linux();

    virtual ~Test_M2MConnectionHandlerImpl_linux();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_listen_for_data();

    void test_data_receive();

    void test_close_connection();


    M2MConnectionHandlerImpl* handler;
    M2MConnection_TestObserver *observer;
};

#endif // TEST_M2M_CONNECTION_HANDLER_IMPL_LINUX_H
