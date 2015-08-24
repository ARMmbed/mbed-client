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
#include "test_m2mreporthandler.h"
#include "m2mobservationhandler.h"
#include "m2mtimer.h"
#include "m2mtimerobserver.h"
#include "m2mbase.h"

class Observer : public M2MReportObserver{

public:

    Observer(){}
    virtual ~Observer(){}
    void observation_to_be_sent(){
        visited = true;
    }
    bool visited;
};

class TimerObserver : public M2MTimerObserver
{
public:
    TimerObserver(){}
    virtual ~TimerObserver(){}

    virtual void timer_expired(M2MTimerObserver::Type ){
        visited = true;
    }

    bool visited;
};

Test_M2MReportHandler::Test_M2MReportHandler()
{
    _observer = new Observer();
    M2MReportHandler handler(*_observer);
    _handler = new M2MReportHandler(*_observer);
}

Test_M2MReportHandler::~Test_M2MReportHandler()
{
    delete _handler;
    delete _observer;
}

void Test_M2MReportHandler::test_set_under_observation()
{
    bool test = true;
    _handler->set_under_observation(test);

    CHECK(test == _handler->_under_observation);
    CHECK(true == _observer->visited);

    _observer->visited = false;

    test = false;
    _handler->set_under_observation(test);
    CHECK(test == _handler->_under_observation);
    CHECK(false == _observer->visited);

    _handler->_pmin = 0.0f;

    _handler->set_under_observation(true);
    CHECK(true == _handler->_under_observation);
}

void Test_M2MReportHandler::test_parse_notification_attribute()
{
    char* val = {"value"};
    CHECK(false == _handler->parse_notification_attribute(val, M2MBase::ObjectInstance ));

    char* val2 = {"value&eval"};
    CHECK(false == _handler->parse_notification_attribute(val2, M2MBase::ObjectInstance ));

    char* val3 = {"toolongvaluevaluevaluevaluevalue&toolongvaluevaluevaluevaluevalue"};
    CHECK(false == _handler->parse_notification_attribute(val3, M2MBase::ObjectInstance ));

    char* val_real = {"st=6&pmax=3&lt=1&gt=100"};
    CHECK(true == _handler->parse_notification_attribute(val_real, M2MBase::Resource ));
    CHECK(true == _handler->_notif_params_set);

    char* val_real1 = {"a=1&pmin=2&pmax=3&gt=4&lt=5&st=6&cancel"};
    CHECK(false == _handler->parse_notification_attribute(val_real1, M2MBase::Resource ));

    char* val2_real = {"cancel&gt=4&lt=5&st=6&cancel"};
    CHECK(false == _handler->parse_notification_attribute(val2_real, M2MBase::Resource ));

    char* val3_real = {"cancel&gt=40&lt=5&st=6&cancel"};
    CHECK(true == _handler->parse_notification_attribute(val3_real, M2MBase::Resource ));

    char* val4_real = {"cancel"};
    CHECK(true == _handler->parse_notification_attribute(val4_real, M2MBase::Resource ));
    CHECK(false == _handler->_notif_params_set);

    char* val_real = {"st=6&pmax=30&lt=1&gt=100&pmin=0"};
    CHECK(true == _handler->parse_notification_attribute(val_real, M2MBase::Resource ));

    _handler->set_value(12);
    _handler->timer_expired(M2MTimerObserver::PMinTimer);
    CHECK(_handler->_pmin_exceeded == true);
}

void Test_M2MReportHandler::test_timer_expired()
{
    _handler->timer_expired(M2MTimerObserver::Notdefined);
    CHECK(_observer->visited == false);

    _handler->_under_observation = true;
    _handler->_pmin_exceeded = true;
    _observer->visited = false;
    _handler->timer_expired(M2MTimerObserver::PMaxTimer);
    CHECK(_handler->_pmax_exceeded == false);
    CHECK(_observer->visited == true);

    _handler->_pmin_exceeded = false;
    _observer->visited = false;

    _handler->timer_expired(M2MTimerObserver::PMinTimer);
    CHECK(_handler->_pmin_exceeded == true);

    _observer->visited = false;
    _handler->_report_scheduled = true;
    _handler->timer_expired(M2MTimerObserver::PMinTimer);
    CHECK(_handler->_pmin_trigger == false);
    CHECK(_observer->visited == true);

    _handler->_under_observation = true;
    _handler->_pmin_exceeded = true;
    _observer->visited = true;
    _handler->timer_expired(M2MTimerObserver::PMinTimer);
    CHECK(_handler->_pmin_exceeded == true);
}

void Test_M2MReportHandler::test_set_value()
{
    _handler->set_value(12);

    _handler->_under_observation = true;
    _handler->set_value(12);

    _handler->_high_step = 100;
    _handler->_low_step = 2;

    _handler->_limits[0] = 10;
    _handler->_limits[1] = 50;

    _handler->set_value(8);

    _handler->_last_band = 2;
    _handler->set_value(1);
    CHECK(_handler->_report_scheduled == true);

    _handler->_pmin_exceeded = true;
    _handler->set_value(0);
    CHECK(_handler->_pmin_exceeded == false);

    _handler->set_value(12);

    _handler->set_value(121);

//    CHECK(set_value(value,(u_int32_t)sizeof(value)) == true);
//    CHECK( this->_value_length == sizeof(value));
//    CHECK( *this->_value == *value);
}
