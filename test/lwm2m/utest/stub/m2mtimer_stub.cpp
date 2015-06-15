/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mtimer_stub.h"

void m2mtimer_stub::clear()
{
}
// Prevents the use of assignment operator
M2MTimer& M2MTimer::operator=(const M2MTimer& /*other*/)
{
    return *this;
}

// Prevents the use of copy constructor
M2MTimer::M2MTimer(const M2MTimer& other)
: _observer(other._observer)
{
    *this = other;
}

M2MTimer::M2MTimer(M2MTimerObserver& observer)
: _observer(observer)
{
}

M2MTimer::~M2MTimer()
{
}

void M2MTimer::start_timer(uint64_t /*interval*/,
                           M2MTimerObserver::Type /*type*/,
                           bool /*single_shot*/)
{
}

void M2MTimer::stop_timer()
{
}

void M2MTimer::timer_expired(M2MTimerObserver::Type /*type*/)
{
}
