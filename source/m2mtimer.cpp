/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "include/m2mtimer.h"
#ifdef TARGET_LIKE_LINUX
    #include "lwm2m-client-linux/m2mtimerimpl.h"
#else
    #include "lwm2m-client-mbed/m2mtimerimpl.h"
#endif
#include "lwm2m-client/m2mconfig.h"

M2MTimer::M2MTimer(M2MTimerObserver& observer)
: _observer(observer),
  _type(M2MTimerObserver::Notdefined)
{
    _impl = new M2MTimerImpl(*this);
}

M2MTimer::~M2MTimer()
{
    delete _impl;
}

void M2MTimer::start_timer(uint64_t interval,
                           M2MTimerObserver::Type type,
                           bool single_shot)
{
    _type = type;
    if(_impl) {
        _impl->start_timer(interval,single_shot);
    }
}

void M2MTimer::stop_timer()
{
    if(_impl) {
        _impl->stop_timer();
    }
}

void M2MTimer::timer_expired(M2MTimerObserver::Type /*type*/)
{
    _observer.timer_expired(_type);
}
