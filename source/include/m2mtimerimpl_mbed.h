/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_IMPL_MBED_H
#define M2M_TIMER_IMPL_MBED_H

#ifndef USE_LINUX

#include <stdint.h>
#include "Ticker.h"

class M2MTimerObserver;
/**
 * @brief M2MTimerImpl
 * Private implementation class for timer, this can be
 * modified based on platform on which mbed client needs
 * to be used.
 */
class M2MTimerImpl
{
public:

    // Prevents the use of assignment operator
    M2MTimerImpl& operator=(const M2MTimerImpl& other);

    // Prevents the use of copy constructor
    M2MTimerImpl(const M2MTimerImpl& other);

    /**
    * Constructor.
    */
    M2MTimerImpl(M2MTimerObserver& _observer);

    /**
    * Destructor.
    */
    virtual ~M2MTimerImpl();

    /**
     * Starts timer
     * @param interval Timer's interval in milliseconds
    * @param single_shot defines if timer is ticked
    * once or is it restarted everytime timer is expired.
    */
    void start_timer(uint64_t interval, bool single_shot = true);

    /**
    * Stops timer.
    * This cancels the ongoing timer.
    */
    void stop_timer();

    /**
    * Callback function for timer completion.
    */
    void timer_expired();

private:

    M2MTimerObserver&   _observer;
    mbed::Ticker        _ticker;
    bool                _single_shot;
    uint64_t            _interval;

friend class Test_M2MTimerImpl_mbed;

};

#endif // #ifndef USE_LINUX

#endif // M2M_TIMER_IMPL_MBED_H
