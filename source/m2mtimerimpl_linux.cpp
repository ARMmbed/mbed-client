/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include <unistd.h>
#include "include/m2mtimerimpl_linux.h"
#include "lwm2m-client/m2mtimerobserver.h"
#include "include/nsdlaccesshelper.h"

M2MTimerImpl& M2MTimerImpl::operator=(const M2MTimerImpl& other)
{
    if( this != &other){
        _single_shot= other._single_shot;
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
  _interval(0),
  _counter(0)
{    
    __timer_impl = this;
}

M2MTimerImpl::~M2MTimerImpl()
{
}

void M2MTimerImpl::start_timer( uint64_t interval,
                                bool single_shot)
{
    _counter = 0;
    _single_shot = single_shot;
    _interval =  interval ;
    pthread_create(&_coap_exec_thread, NULL,__thread_poll_function, NULL);
}

void M2MTimerImpl::stop_timer()
{
    pthread_cancel(_coap_exec_thread);
    _interval = 0;
    _counter = 0;
    _single_shot = false;
}

void M2MTimerImpl::timer_expired()
{
    pthread_join(_coap_exec_thread,NULL);
    _observer.timer_expired();
    if(_single_shot) {
        start_timer(_interval,_single_shot);
    }
}

void M2MTimerImpl::thread_function()
{
    while(1) {
        usleep(1000);
        _counter+= 1000;
        if(_counter >= _interval){
            break;
        }
    }
    timer_expired();
}
