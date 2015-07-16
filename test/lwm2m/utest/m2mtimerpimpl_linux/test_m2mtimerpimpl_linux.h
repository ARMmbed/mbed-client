/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TIMER_PIMPL_LINUX_H
#define TEST_M2M_TIMER_PIMPL_LINUX_H

#include "lwm2m-client-linux/m2mtimerpimpl.h"

class TestObserver;

class Test_M2MTimerPimpl_linux
{
public:
    Test_M2MTimerPimpl_linux();

    virtual ~Test_M2MTimerPimpl_linux();

    void test_start_timer();

    void test_stop_timer();

    void test_timer_expired();

    void test_run();

    void test_start_dtls_timer();

    void test_is_intermediate_interval_passed();

    void test_is_total_interval_passed();

    M2MTimerPimpl* timer;
    TestObserver* observer;
};

#endif // TEST_M2M_TIMER_IMPL_LINUX_H
