/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
