/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_REPORT_HANDLER_H
#define TEST_M2M_REPORT_HANDLER_H

#include "m2mreporthandler.h"

class Observer;

class Test_M2MReportHandler
{
public:    
    Test_M2MReportHandler();

    virtual ~Test_M2MReportHandler();

    void test_set_under_observation();

    void test_parse_notification_attribute();

    void test_timer_expired();

    void test_set_value();

    M2MReportHandler *_handler;
    Observer         * _observer;
};


#endif // TEST_M2M_BASE_H
