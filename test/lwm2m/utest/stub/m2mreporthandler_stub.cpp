/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mreporthandler_stub.h"

bool m2mreporthandler_stub::bool_return;

void m2mreporthandler_stub::clear()
{
    bool_return = false;
}

M2MReportHandler::M2MReportHandler(M2MReportObserver &observer)
: _observer(observer)
{
}

M2MReportHandler::~M2MReportHandler()
{
}

void M2MReportHandler::set_under_observation(bool)
{
}

void M2MReportHandler::set_value(float)
{
}

bool M2MReportHandler::parse_notification_attribute(char *&,
                                                    M2MBase::BaseType )
{
    return m2mreporthandler_stub::bool_return;
}

void M2MReportHandler::timer_expired(M2MTimerObserver::Type )
{
}
