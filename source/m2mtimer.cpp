#include "m2mtimer.h"
#ifdef USE_LINUX
    #include "m2mtimerimpl_linux.h"
#else
    #include "m2mtimerimpl_mbed.h"
#endif

// Prevents the use of assignment operator
M2MTimer& M2MTimer::operator=(const M2MTimer& other)
{
    if( this != &other){
        _type = other._type;
        if( other._impl){
        _impl = new M2MTimerImpl(*other._impl);
        }
    }
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
