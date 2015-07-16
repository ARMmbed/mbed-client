/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_CONNECTION_HANDLER_FACTORY_H
#define TEST_M2M_CONNECTION_HANDLER_FACTORY_H

#include "m2mconnectionhandlerfactory.h"

class Test_M2MConnectionHandlerFactory
{
public:
    Test_M2MConnectionHandlerFactory();

    virtual ~Test_M2MConnectionHandlerFactory();

    void test_createConnectionHandler();
};

#endif // TEST_M2M_CONNECTION_HANDLER_FACTORY_H
