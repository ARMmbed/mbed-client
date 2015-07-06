/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerimpl_linux.h"
#include "common_stub.h"

class TestObserver : public M2MTimerObserver {

public:
    TestObserver(){}
    virtual ~TestObserver(){}
    void timer_expired(M2MTimerObserver::Type){
        visited = true;
    }
    bool visited;

};

Test_M2MTimerImpl_linux::Test_M2MTimerImpl_linux()
{
    observer = new TestObserver();
    timer = new M2MTimerImpl(*observer);
}


Test_M2MTimerImpl_linux::~Test_M2MTimerImpl_linux()
{
    common_stub::clear();
    delete observer;
    common_stub::int2_value = 0;
    delete timer;
}

void Test_M2MTimerImpl_linux::test_start_timer()
{
    timer->start_timer(100,true);
    CHECK(timer->_started == 1);

    timer->_started = 1;
    timer->start_timer(100,true);
}

void Test_M2MTimerImpl_linux::test_stop_timer()
{
    timer->_started = 1;
    common_stub::int_value = 0;
    common_stub::int2_value = 1;
    timer->stop_timer();
    CHECK(timer->_started == 0);
}

void Test_M2MTimerImpl_linux::test_timer_expired()
{
    timer->timer_expired();
    CHECK(observer->visited == true);

    timer->_single_shot = false;
    timer->timer_expired();
}

void Test_M2MTimerImpl_linux::test_run()
{
    timer->run();
    CHECK(observer->visited == true);
}
