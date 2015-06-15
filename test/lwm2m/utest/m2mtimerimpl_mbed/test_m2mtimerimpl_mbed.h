/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TIMER_IMPL_MBED_H
#define TEST_M2M_TIMER_IMPL_MBED_H

#include "lwm2m-client-mbed/m2mtimerimpl.h"

class TestObserver;

class Test_M2MTimerImpl_mbed
{
public:
    Test_M2MTimerImpl_mbed();

    void test_copy_constructor();

    virtual ~Test_M2MTimerImpl_mbed();

    void test_start_timer();

    void test_stop_timer();

    void test_timer_expired();

    M2MTimerImpl* timer;
    TestObserver* observer;
};

#endif // TEST_M2M_TIMER_IMPL_MBED_H
