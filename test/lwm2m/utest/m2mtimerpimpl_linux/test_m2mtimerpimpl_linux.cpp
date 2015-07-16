/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerpimpl_linux.h"
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

Test_M2MTimerPimpl_linux::Test_M2MTimerPimpl_linux()
{
    observer = new TestObserver();
    timer = new M2MTimerPimpl(*observer);
}


Test_M2MTimerPimpl_linux::~Test_M2MTimerPimpl_linux()
{
    common_stub::clear();
    delete observer;
    common_stub::int2_value = 0;
    delete timer;
}

void Test_M2MTimerPimpl_linux::test_start_timer()
{
    timer->start_timer(100,M2MTimerObserver::Notdefined,true);

//    timer->start_timer(100,M2MTimerObserver::Notdefined,true);
}

void Test_M2MTimerPimpl_linux::test_stop_timer()
{
    common_stub::int_value = 0;
    common_stub::int2_value = 1;;
    timer->stop_timer();
}

void Test_M2MTimerPimpl_linux::test_timer_expired()
{
    timer->timer_expired();
    CHECK(observer->visited == true);

    timer->_single_shot = false;
    timer->timer_expired();
}

void Test_M2MTimerPimpl_linux::test_run()
{
    timer->run();
    CHECK(observer->visited == true);

    timer->_dtls_type = true;
    timer->run();
    CHECK(2 == timer->_status);
}

void Test_M2MTimerPimpl_linux::test_start_dtls_timer()
{
    timer->start_dtls_timer(10, 100, M2MTimerObserver::Dtls);
}

void Test_M2MTimerPimpl_linux::test_is_intermediate_interval_passed()
{
    CHECK(false == timer->is_intermediate_interval_passed());
    timer->_status = 1;
    CHECK(true == timer->is_intermediate_interval_passed());
}

void Test_M2MTimerPimpl_linux::test_is_total_interval_passed()
{
    CHECK(false == timer->is_total_interval_passed());
    timer->_status = 2;
    CHECK(true == timer->is_total_interval_passed());
}
