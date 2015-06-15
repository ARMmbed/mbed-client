/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mtimerimpl_stub.h"

bool m2mtimerimpl_stub::visited;

void m2mtimerimpl_stub::clear()
{
    visited = false;
}


M2MTimerImpl& M2MTimerImpl::operator=(const M2MTimerImpl& other)
{
    return *this;
}

// Prevents the use of copy constructor
M2MTimerImpl::M2MTimerImpl(const M2MTimerImpl& other)
:_observer(other._observer)
{
    *this = other;
}

M2MTimerImpl::M2MTimerImpl(M2MTimerObserver& observer)
: _observer(observer)
{

}

M2MTimerImpl::~M2MTimerImpl()
{
}

void M2MTimerImpl::start_timer( uint64_t,bool )
{
    m2mtimerimpl_stub::visited = true;
}

void M2MTimerImpl::stop_timer()
{
    m2mtimerimpl_stub::visited = true;
}

void M2MTimerImpl::timer_expired()
{
}

