/*
 * Copyright (c) 2015 ARM. All rights reserved.
*/
#ifndef M2MREPORTHANDLER_H
#define M2MREPORTHANDLER_H

// Support for std args
#include <stdint.h>
#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2mbase.h"
#include "lwm2m-client/m2mtimerobserver.h"

//algorithm can accept any number of limit values and report when signal changes between limit bands
#define MAX_LIMITS 2

//FORWARD DECLARATION
class M2MReportObserver;
class M2MTimer;

/**
 *  @brief M2MReportHandler.
 *  This class is handles all the observation related operations.
 */

class M2MReportHandler: public M2MTimerObserver {

public:

    // Prevents the use of default constructor.
    M2MReportHandler(M2MReportObserver &observer);

public:

    /**
     * Destructor
     */
    virtual ~M2MReportHandler();

    /**
     * @brief Sets that object is under observation.
     * @param Value for the observation.
     * @param handler, Handler object for sending
     * observation callbacks.
     */
    virtual void set_under_observation(bool observed);

    /**
     * @brief Sets the value of the given resource.
     * @param value, Valuye of the observed resource.
     */
    virtual void set_value(float value);

    /**
     * @brief Parses the received query for notification
     * attribute.
     * @param query Query to be parsed for attributes.
     * @param type Type of the Base Object.
     * @return true if required attributes are present else false.
     */
    virtual bool parse_notification_attribute(char *&query,
                                              M2MBase::BaseType type);

protected : // from M2MTimerObserver

    virtual void timer_expired(M2MTimerObserver::Type type =
                               M2MTimerObserver::Notdefined);

private:

    bool set_notification_attribute(char* option, M2MBase::BaseType type);

    /**
     * @brief Schedule a report, if the pmin is exceeded
     * then report immediately else store the state to be
     * reported once the time fires.
     */
    void schedule_report(float value);

    /**
    * @brief Reports a sample that satisfies the reporting criteria.
    */
    void report(float value);

    /**
    * @brief Determine which band [0..num_limits] the provided sample is in
    * works with any number of bands 2 to MAX_LIMITS+1
    */
    int band(float value);

private:

    M2MReportObserver           &_observer;
    bool                        _under_observation;
    float                       _value;
    float                       _pmax;
    float                       _pmin;
    float                       _gt;
    float                       _lt;
    float                       _st;
    bool                        _pmin_exceeded;
    bool                        _pmin_trigger;
    bool                        _pmax_exceeded;
    bool                        _report_scheduled;
    M2MTimer                    *_pmin_timer;
    M2MTimer                    *_pmax_timer;
    int                         _num_limits;
    float                       _limits[MAX_LIMITS];
    int                         _last_band;
    float                       _high_step;
    float                       _low_step;
    float                       _current_value;
    float                       _last_value;

friend class Test_M2MReportHandler;

};

#endif // M2MREPORTHANDLER_H
