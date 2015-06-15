/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerimpl_mbed.h"
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

Test_M2MTimerImpl_mbed::Test_M2MTimerImpl_mbed()
{
    observer = new TestObserver();
    timer = new M2MTimerImpl(*observer);
}

void Test_M2MTimerImpl_mbed::test_copy_constructor()
{
    M2MTimerImpl* copy = new M2MTimerImpl(*timer);

    copy->start_timer(100,true);
    delete copy;
}

Test_M2MTimerImpl_mbed::~Test_M2MTimerImpl_mbed()
{
    common_stub::clear();
    delete observer;
    delete timer;
}

void Test_M2MTimerImpl_mbed::test_start_timer()
{
    timer->start_timer(100,true);
}

void Test_M2MTimerImpl_mbed::test_stop_timer()
{
    timer->stop_timer();
}

void Test_M2MTimerImpl_mbed::test_timer_expired()
{
    timer->timer_expired();
    CHECK(observer->visited == true);

    timer->_single_shot = false;
    timer->timer_expired();
}
