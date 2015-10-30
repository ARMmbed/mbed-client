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

void M2MReportHandler::trigger_object_notification()
{
}

void M2MReportHandler::set_string_notification_trigger()
{
}
