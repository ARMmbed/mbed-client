/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtimer.h"
#include "m2mtimerimpl_stub.h"

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

void Test_M2MTimer::test_copy_constructor()
{
    M2MTimer* copy = new M2MTimer(*timer);

    copy->start_timer(100, M2MTimerObserver::Notdefined,true);
    CHECK(m2mtimerimpl_stub::visited == true);
    delete copy;
}

Test_M2MTimer::~Test_M2MTimer()
{
    m2mtimerimpl_stub::clear();
    delete observer;
    delete timer;
}

void Test_M2MTimer::test_start_timer()
{
    timer->start_timer(100, M2MTimerObserver::Notdefined,true);
    CHECK(m2mtimerimpl_stub::visited == true);
}

void Test_M2MTimer::test_stop_timer()
{
    timer->stop_timer();
    CHECK(m2mtimerimpl_stub::visited == true);
}

void Test_M2MTimer::test_timer_expired()
{
    timer->timer_expired(M2MTimerObserver::Notdefined);
    CHECK(observer->visited == true);
}
