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
#include "m2mtimerpimpl_stub.h"
bool m2mtimerpimpl_stub::bool_value;
bool m2mtimerpimpl_stub::visited;

void m2mtimerpimpl_stub::clear()
{
    bool_value = false;
    visited = false;
}

M2MTimerPimpl::M2MTimerPimpl(M2MTimerObserver& observer)
: _observer(observer)
{

}

M2MTimerPimpl::~M2MTimerPimpl()
{
}

void M2MTimerPimpl::start_timer( uint64_t,
                                 M2MTimerObserver::Type,
                                 bool )
{
    m2mtimerpimpl_stub::visited = true;
}

void M2MTimerPimpl::start_dtls_timer(uint64_t intermediate_interval, uint64_t total_interval, M2MTimerObserver::Type type)
{
    m2mtimerpimpl_stub::visited = true;
}

void M2MTimerPimpl::stop_timer()
{
    m2mtimerpimpl_stub::visited = true;
}

void M2MTimerPimpl::timer_expired()
{
}

void M2MTimerPimpl::run()
{
}

bool M2MTimerPimpl::is_intermediate_interval_passed()
{
    return m2mtimerpimpl_stub::bool_value;
}

bool M2MTimerPimpl::is_total_interval_passed()
{
    return m2mtimerpimpl_stub::bool_value;
}
