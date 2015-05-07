/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2mreportobserver.h"
#include "lwm2m-client/m2mconstants.h"
#include "include/m2mtimer.h"
#include "include/m2mreporthandler.h"

M2MReportHandler::M2MReportHandler(M2MReportObserver &observer)
: _observer(observer),
  _under_observation(false),
  _value(0.0f),
  _pmax(0.0f),
  _pmin(1.0f),
  _gt(0.0f),
  _lt(0.0f),
  _st(0.0f),
  _pmin_exceeded(false),
  _pmin_trigger(false),
  _pmax_exceeded(false),
  _report_scheduled(false),
  _pmin_timer(NULL),
  _pmax_timer(NULL),
  _num_limits(2),
  _last_band(0),
  _high_step(0.0f),
  _low_step(0.0f),
  _current_value(0.0f),
  _last_value(0.0f)
{
}

M2MReportHandler::~M2MReportHandler()
{
    if(_pmax_timer) {
        delete _pmax_timer;
    }
    if(_pmin_timer) {
    delete _pmin_timer;
    }
}

void M2MReportHandler::set_under_observation(bool observed)
{
    _under_observation = observed;
    _report_scheduled = false;
    if(observed) {
        // initializes and sends an update if observing is on, don't change observing state
        // allows cancel to turn off observing and update state without sending a notification
        _report_scheduled = true;
        report(_current_value);
    }
}

void M2MReportHandler::set_value(float value)
{
    _current_value = value;
    if(_current_value != _last_value && _under_observation) {
        _last_value = _current_value;

        if (band(_current_value) != _last_band ||
            _current_value >= _high_step       ||
            _current_value <= _low_step) { // test limits
            // schedule reporting
            schedule_report(_current_value);
        }
    }
}

bool M2MReportHandler::parse_notification_attribute(char *&query,
                                                    M2MBase::BaseType type)
{
    bool success = false;
    char *query_option = strtok(query, "&");// split the string
    char query_options[5][20];
    uint8_t num_options = 0;
    while (query_option != NULL){
        strcpy(query_options[num_options++], query_option);
        query_option = strtok(NULL, "&");// next query option
    }
    for (int option = 0; option < num_options; option++) {
        if(set_notification_attribute(query_options[option],type)) {
            success = true;
        }
    }
    return success;
}

void M2MReportHandler::timer_expired(M2MTimerObserver::Type type)
{
    switch(type) {
        case M2MTimerObserver::PMinTimer: {
            if (_report_scheduled){
                _report_scheduled = false;
                _pmin_trigger = true;
                report(_current_value);
            }
            else{
                _pmin_exceeded = true;
            }
        }
        break;
        case M2MTimerObserver::PMaxTimer: {
            _pmax_exceeded = true;
            report(_current_value);
        }
        break;
        default:
            break;
    }
}

bool M2MReportHandler::set_notification_attribute(char* option,
                                                  M2MBase::BaseType type)
{
    bool success = false;
    bool observation = true;
    char* attribute = strtok(option, EQUAL.c_str()); // first token
    char* value = strtok(NULL, EQUAL.c_str()); // next token

    if (strcmp(attribute, PMIN.c_str()) == 0) {
        sscanf(value, "%f", &_pmin);
        success = true;
    }
    else if(strcmp(attribute, PMAX.c_str()) == 0) {
        sscanf(value, "%f", &_pmax);
        success = true;
    }
    else if(strcmp(attribute, GT.c_str()) == 0 &&
            M2MBase::Resource == type){
        sscanf(value, "%f", &_gt);
        success = true;
    }
    else if(strcmp(attribute, LT.c_str()) == 0 &&
            M2MBase::Resource == type){
        sscanf(value, "%f", &_lt);
        success = true;
    }
    else if(strcmp(attribute, ST.c_str()) == 0 &&
            M2MBase::Resource == type){
        sscanf(value, "%f", &_st);
        success = true;
    }
    else if(strcmp(attribute, CANCEL.c_str()) == 0) {
        observation = false;
        success = true;
    }
    if(success) {
        set_under_observation(observation);
    }
    return success;
}

void M2MReportHandler::schedule_report(float value)
{
    if(_under_observation) {
        if (_pmin_exceeded) {
            report(value);
        }
        else {
            _report_scheduled = true;
        }
    }
}

void M2MReportHandler::report(float value)
{
    if(_under_observation) {
        _pmin_exceeded = false;
        _pmax_exceeded = false;
        _pmin_trigger = false;
        _last_band = band(value);
        _high_step = value + _st;
        _low_step = value - _st;
        _observer.observation_to_be_sent();
    }

    uint64_t time_interval = 0;
    if(_pmin != 0) {
        time_interval = (uint64_t)(_pmin * 1000);
        if(!_pmin_timer) {
            _pmin_timer = new M2MTimer(*this);
        }
        _pmin_timer->stop_timer();
        _pmin_timer->start_timer(time_interval,
                                 M2MTimerObserver::PMinTimer,
                                 true);
    }
    if(_pmax != 0) {
        time_interval = (uint64_t)(_pmax * 1000);
        if(!_pmax_timer) {
            _pmax_timer = new M2MTimer(*this);
        }
        _pmax_timer->stop_timer();
        _pmax_timer->start_timer(time_interval,
                                 M2MTimerObserver::PMaxTimer,
                                 true);
    }
}

int M2MReportHandler::band(float sample)
{
    if (sample > _limits[_num_limits-1]) {
        return _num_limits;
    }
    else {
    for ( int limit = 0; limit < _num_limits; limit++ ) {
        if (sample <= _limits[limit]) {
            return limit;
            }
        }
    }
    return -1;
}
