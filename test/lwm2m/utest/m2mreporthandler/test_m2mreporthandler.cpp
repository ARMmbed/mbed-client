/*
 * Copyright (c) 2015 ARM. All rights reserved.
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
}

void Test_M2MReportHandler::test_parse_notification_attribute()
{
    char* val = {"value"};
    CHECK(false == _handler->parse_notification_attribute(val, M2MBase::ObjectInstance ));

    char* val2 = {"value&eval"};
    CHECK(false == _handler->parse_notification_attribute(val2, M2MBase::ObjectInstance ));

    char* val3 = {"toolongvaluevaluevaluevaluevalue&toolongvaluevaluevaluevaluevalue"};
    CHECK(false == _handler->parse_notification_attribute(val3, M2MBase::ObjectInstance ));

    char* val_real = {"st=6"};
    CHECK(true == _handler->parse_notification_attribute(val_real, M2MBase::Resource ));

    char* val_real1 = {"a=1&pmin=2&pmax=3&gt=4&lt=5&st=6&cancel"};
    CHECK(true == _handler->parse_notification_attribute(val_real1, M2MBase::Resource ));

    char* val2_real = {"cancel&gt=4&lt=5&st=6&cancel"};
    CHECK(true == _handler->parse_notification_attribute(val2_real, M2MBase::Resource ));

}

void Test_M2MReportHandler::test_timer_expired()
{
    _handler->timer_expired(M2MTimerObserver::Notdefined);
    CHECK(_observer->visited == false);

    _handler->_under_observation = true;
    _observer->visited = false;
    _handler->timer_expired(M2MTimerObserver::PMaxTimer);
    CHECK(_handler->_pmax_exceeded == false);
    CHECK(_observer->visited == true);

    _observer->visited = false;

    _handler->timer_expired(M2MTimerObserver::PMinTimer);
    CHECK(_handler->_pmin_exceeded == true);

    _observer->visited = false;
    _handler->_report_scheduled = true;
    _handler->timer_expired(M2MTimerObserver::PMinTimer);
    CHECK(_handler->_pmin_trigger == false);
    CHECK(_observer->visited == true);

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
