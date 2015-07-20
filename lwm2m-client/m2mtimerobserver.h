/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_OBSERVER_H
#define M2M_TIMER_OBSERVER_H

/**
 *  @brief M2MTimerObserver
 *  Observer class for informing timer expiry to the parent class
 */
class M2MTimerObserver
{
public:
    /**
      * @enum Defines the types of timer
      * that can be created for LWM2M Client.
      */
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
    * @param type, Type of timer that has expired.
    */
    virtual void timer_expired(M2MTimerObserver::Type type =
                               M2MTimerObserver::Notdefined) = 0;
};

#endif // M2M_TIMER_OBSERVER_H
