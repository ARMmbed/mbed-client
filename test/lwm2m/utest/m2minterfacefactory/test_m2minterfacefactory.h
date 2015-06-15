/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_SECURITY_H
#define TEST_M2M_SECURITY_H

#include "m2minterfacefactory.h"

class Test_M2MInterfaceFactory
{

public:
    Test_M2MInterfaceFactory();
    virtual ~Test_M2MInterfaceFactory();

    void test_create_interface();

    void test_create_security();

    void test_create_device();

    void test_create_server();

    void test_create_object();
};

#endif // TEST_M2M_SECURITY_H
