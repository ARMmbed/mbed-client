/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtimer.h"
#include "m2mtimerpimpl_stub.h"

class TestObserver : public M2MTimerObserver {

public:
    TestObserver(){}
    virtual ~TestObserver(){}
    void timer_expired(M2MTimerObserver::Type){
        visited = true;
    }
    bool visited;

};

Test_M2MTimer::Test_M2MTimer()
{
    observer = new TestObserver();
    timer = new M2MTimer(*observer);
}

Test_M2MTimer::~Test_M2MTimer()
{
    m2mtimerpimpl_stub::clear();
    delete observer;
    delete timer;
}

void Test_M2MTimer::test_start_timer()
{
    timer->start_timer(100, M2MTimerObserver::Notdefined,true);
    CHECK(m2mtimerpimpl_stub::visited == true);
}

void Test_M2MTimer::test_stop_timer()
{
    timer->stop_timer();
    CHECK(m2mtimerpimpl_stub::visited == true);
}

void Test_M2MTimer::test_start_dtls_timer()
{
    timer->start_dtls_timer(100, 110);
    CHECK(m2mtimerpimpl_stub::visited == true);
}

void Test_M2MTimer::test_is_intermediate_interval_passed()
{
    m2mtimerpimpl_stub::bool_value = true;
    CHECK(true == timer->is_intermediate_interval_passed());
}

void Test_M2MTimer::test_is_total_interval_passed()
{
    m2mtimerpimpl_stub::bool_value = true;
    CHECK(true == timer->is_total_interval_passed());
}
