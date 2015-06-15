/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_INTERFACE_IMPL_H
#define TEST_M2M_INTERFACE_IMPL_H

#include "m2minterfaceimpl.h"

class TestObserver;

class Test_M2MInterfaceImpl
{
public:
    Test_M2MInterfaceImpl();

    virtual ~Test_M2MInterfaceImpl();

    void test_bootstrap();

    void test_cancel_bootstrap();

    void test_register_object();

    void test_update_registration();

    void test_unregister_object();

    void test_coap_message_ready();

    void test_client_registered();

    void test_registration_updated();

    void test_registration_error();

    void test_client_unregistered();

    void test_bootstrap_done();

    void test_bootstrap_error();

    void test_coap_data_processed();

    void test_value_updated();

    void test_data_available();

    void test_socket_error();

    void test_address_ready();

    void test_data_sent();

    M2MInterfaceImpl*   impl;
    TestObserver        *observer;
};

#endif // TEST_M2M_INTERFACE_IMPL_H
