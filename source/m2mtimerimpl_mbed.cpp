/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "include/m2mtimerimpl_mbed.h"
#include "lwm2m-client/m2mtimerobserver.h"

M2MTimerImpl& M2MTimerImpl::operator=(const M2MTimerImpl& other)
{
    if( this != &other){
        _single_shot= other._single_shot;
        //_ticker = other._ticker;
        _interval = other._interval;
    }
    return *this;
}

// Prevents the use of copy constructor
M2MTimerImpl::M2MTimerImpl(const M2MTimerImpl& other)
:_observer(other._observer)
{
    *this = other;
}

M2MTimerImpl::M2MTimerImpl(M2MTimerObserver& observer)
: _observer(observer),
  _single_shot(true),
  _interval(0)
{
}

M2MTimerImpl::~M2MTimerImpl()
{
}

void M2MTimerImpl::start_timer( uint64_t interval,
                                bool single_shot)
{
    _single_shot = single_shot;
    _interval =  interval ;
    _ticker.detach();
    _ticker.attach_us(this,
                      &M2MTimerImpl::timer_expired,
                      _interval * 1000);
}

void M2MTimerImpl::stop_timer()
{
    _interval = 0;
    _single_shot = false;
    _ticker.detach();
}

void M2MTimerImpl::timer_expired()
{
    _observer.timer_expired();
    if(!_single_shot) {
        start_timer(_interval,true);
    }
}
