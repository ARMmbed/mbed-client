/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TIMER_IMPL_LINUX_H
#define TEST_M2M_TIMER_IMPL_LINUX_H

#include "lwm2m-client-linux/m2mtimerimpl.h"

class TestObserver;

class Test_M2MTimerImpl_linux
{
public:
    Test_M2MTimerImpl_linux();

    virtual ~Test_M2MTimerImpl_linux();

    void test_start_timer();

    void test_stop_timer();

    void test_timer_expired();

    void test_run();

    M2MTimerImpl* timer;
    TestObserver* observer;
};

#endif // TEST_M2M_TIMER_IMPL_LINUX_H
