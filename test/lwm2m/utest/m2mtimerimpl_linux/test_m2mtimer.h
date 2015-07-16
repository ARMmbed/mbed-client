/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TIMER_H
#define TEST_M2M_TIMER_H

#include "m2mtimer.h"

class TestObserver;

class Test_M2MTimer
{
public:
    Test_M2MTimer();

    virtual ~Test_M2MTimer();

    void test_start_timer();

    void test_stop_timer();

    void test_start_dtls_timer();

    void test_is_intermediate_interval_passed();

    void test_is_total_interval_passed();

    M2MTimer* timer;
    TestObserver* observer;
};

#endif // TEST_M2M_TIMER_H
