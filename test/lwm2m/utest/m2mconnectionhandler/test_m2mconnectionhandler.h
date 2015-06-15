/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_HANDLER_H
#define TEST_M2M_CONNECTION_HANDLER_H

#include "m2mconnectionhandler.h"

class Test_M2MConnectionHandler
{
public:
    Test_M2MConnectionHandler();

    virtual ~Test_M2MConnectionHandler();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_listen_for_data();

    void test_close_connection();

    M2MConnectionHandler* handler;
};

#endif // TEST_M2M_CONNECTION_HANDLER_H
