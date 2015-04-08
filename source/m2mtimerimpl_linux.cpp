/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include <unistd.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "include/m2mtimerimpl_linux.h"
#include "lwm2m-client/m2mtimerobserver.h"
#include "include/nsdlaccesshelper.h"

#define SLOT_NSECS (50000)

M2MTimerImpl& M2MTimerImpl::operator=(const M2MTimerImpl& other)
{
    if( this != &other){
        _single_shot= other._single_shot;
        _interval = other._interval;
        _remaining = other._remaining;
        _started = other._started;
        _mtx = other._mtx;
        _rem_mtx = other._rem_mtx;
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
  _remaining(0),
  _started(0),
  _mtx(PTHREAD_MUTEX_INITIALIZER),
  _rem_mtx(PTHREAD_MUTEX_INITIALIZER)
{    
    __timer_impl = this;
}

M2MTimerImpl::~M2MTimerImpl()
{
}

void M2MTimerImpl::start_timer( uint64_t interval,
                                bool single_shot)
{
    _single_shot = single_shot;
    _interval =  interval ;
    pthread_mutex_lock(&_mtx);

    if (!pthread_equal(_timer_th, pthread_self()))
    {
        if (_started)
            stop_timer();
        pthread_create(&_timer_th, NULL, __thread_poll_function, NULL);
        _started = 1;
    }
    _slot_sleep.tv_sec = 0;
    _slot_sleep.tv_nsec = SLOT_NSECS * interval;
    pthread_mutex_unlock(&_mtx);
}


void M2MTimerImpl::stop_timer()
{
    _started = 0;
    if (!pthread_equal(_timer_th, pthread_self())) {

        if (0 == pthread_cancel(_timer_th)) {
            pthread_join(_timer_th, NULL);
            pthread_mutex_unlock(&_rem_mtx);
        }
    }
    _interval = 0;
    _single_shot = false;
}

void M2MTimerImpl::timer_expired()
{
    _observer.timer_expired();
    if(_single_shot) {
        start_timer(_interval,_single_shot);
    }
}

void M2MTimerImpl::thread_function()
{    
    int rc;
    struct timespec rem;

    while (_started) {
        pthread_mutex_lock(&_mtx);
        rem = _slot_sleep;
        pthread_mutex_unlock(&_mtx);
        do {
            pthread_mutex_lock(&_rem_mtx);
            rc = nanosleep(&rem, &rem); /* Cancellation point, leaves rem_mtx locked */
            _remaining = (uint16_t)(rem.tv_nsec / SLOT_NSECS);
            pthread_mutex_unlock(&_rem_mtx);
        } while ((-1 == rc) && (EINTR==errno)); /* Sleep again, if interrupted by signal */
            ;;
        timer_expired();
    }
}

uint16_t M2MTimerImpl::platform_timer_get_remaining_slots(void)
{
    uint16_t ret;
    pthread_mutex_lock(&_rem_mtx);
    ret = _remaining;
    pthread_mutex_unlock(&_rem_mtx);
    return ret;
}
