/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TIMER_IMPL_MBED_H
#define TEST_M2M_TIMER_IMPL_MBED_H

#include "lwm2m-client-mbed/m2mtimerpimpl.h"

class TestObserver;

class Test_M2MTimerPimpl_mbed
{
public:
    Test_M2MTimerPimpl_mbed();

    virtual ~Test_M2MTimerPimpl_mbed();

    void test_start_timer();

    void test_stop_timer();

    void test_timer_expired();

    void test_start_dtls_timer();

    void test_dtls_timer_expired();

    void test_is_intermediate_interval_passed();

    void test_is_total_interval_passed();

    M2MTimerPimpl* timer;
    TestObserver* observer;
};

#endif // TEST_M2M_TIMER_IMPL_MBED_H
