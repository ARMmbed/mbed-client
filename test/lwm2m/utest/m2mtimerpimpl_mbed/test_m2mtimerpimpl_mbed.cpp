/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerpimpl_mbed.h"
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

Test_M2MTimerPimpl_mbed::Test_M2MTimerPimpl_mbed()
{
    observer = new TestObserver();
    timer = new M2MTimerPimpl(*observer);
}

Test_M2MTimerPimpl_mbed::~Test_M2MTimerPimpl_mbed()
{
    common_stub::clear();
    delete observer;
    delete timer;
}

void Test_M2MTimerPimpl_mbed::test_start_timer()
{
    timer->start_timer(100,M2MTimerObserver::Notdefined,true);
}

void Test_M2MTimerPimpl_mbed::test_stop_timer()
{
    timer->stop_timer();
}

void Test_M2MTimerPimpl_mbed::test_timer_expired()
{
    timer->_single_shot = true;
    timer->timer_expired();
    CHECK(observer->visited == true);

    timer->_single_shot = false;
    timer->timer_expired();
}

void Test_M2MTimerPimpl_mbed::test_start_dtls_timer()
{
    timer->start_dtls_timer(10,100,M2MTimerObserver::Dtls);

    timer->_status = 6;
    timer->start_dtls_timer(10,100,M2MTimerObserver::Dtls);
}

void Test_M2MTimerPimpl_mbed::test_dtls_timer_expired()
{
    timer->_status = 0;
    timer->dtls_timer_expired();
    CHECK(1 == timer->_status);

    timer->_status = 1;
    timer->dtls_timer_expired();
    CHECK(2 == timer->_status);
}

void Test_M2MTimerPimpl_mbed::test_is_intermediate_interval_passed()
{
    CHECK(false == timer->is_intermediate_interval_passed());
    timer->_status = 1;
    CHECK(true == timer->is_intermediate_interval_passed());
}

void Test_M2MTimerPimpl_mbed::test_is_total_interval_passed()
{
    CHECK(false == timer->is_total_interval_passed());
    timer->_status = 2;
    CHECK(true == timer->is_total_interval_passed());
}
