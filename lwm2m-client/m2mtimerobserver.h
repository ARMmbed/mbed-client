/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_OBSERVER_H
#define M2M_TIMER_OBSERVER_H

/**
 *  Observer class for informing timer expiry to the parent class
 */
class M2MTimerObserver
{
public:
    typedef enum {
        Notdefined,
        Registration,
        NsdlExecution,
        PMinTimer,
        PMaxTimer,
        Dtls
    }Type;

    /**
    * Indicates that the time has expired.
    */
    virtual void timer_expired(M2MTimerObserver::Type type =
                               M2MTimerObserver::Notdefined) = 0;
};

#endif // M2M_TIMER_OBSERVER_H
