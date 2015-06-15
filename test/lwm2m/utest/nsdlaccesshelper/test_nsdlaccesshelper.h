/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_NSDL_ACCESS_HELPER_H
#define TEST_NSDL_ACCESS_HELPER_H

#include "nsdlaccesshelper.h"
#include "common_stub.h"

class TestObserver;

class Test_NsdlAccessHelper
{
public:
    Test_NsdlAccessHelper();

    virtual ~Test_NsdlAccessHelper();

    void test_nsdl_c_callback();

    void test_nsdl_c_memory_alloc();

    void test_nsdl_c_memory_free();

    void test_nsdl_c_send_to_server();

    void test_nsdl_c_received_from_server();

    void test_nsdl_c_bootstrap_done();

    void test_socket_malloc();

    void test_socket_free();

    TestObserver *observer;
};

#endif // TEST_NSDL_ACCESS_HELPER_H
