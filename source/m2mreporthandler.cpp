/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-client/m2mreportobserver.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mtimer.h"
#include "include/m2mreporthandler.h"
#include "ns_trace.h"
#include <string.h>

M2MReportHandler::M2MReportHandler(M2MReportObserver &observer)
: _observer(observer),
  _under_observation(false),  
  _pmax(-1.0f),
  _pmin(1.0f),
  _gt(0.0f),
  _lt(0.0f),
  _st(0.0f),
  _pmin_exceeded(false),
  _pmax_exceeded(false),
  _report_scheduled(false),
  _pmin_timer(NULL),
  _pmax_timer(NULL),  
  _high_step(0.0f),
  _low_step(0.0f),
  _current_value(0.0f),
  _last_value(0.0f),
  _attribute_state(0),
  _value_not_in_range(false)
{
    tr_debug("M2MReportHandler::M2MReportHandler()");
    _pmax_timer = new M2MTimer(*this);
    _pmin_timer = new M2MTimer(*this);
}

M2MReportHandler::~M2MReportHandler()
{
    tr_debug("M2MReportHandler::~M2MReportHandler()");
    if(_pmax_timer) {
        delete _pmax_timer;
    }
    if(_pmin_timer) {
        delete _pmin_timer;
    }
}

void M2MReportHandler::set_under_observation(bool observed)
{
    tr_debug("M2MReportHandler::set_under_observation(observed %d)", (int)observed);
    _under_observation = observed;
    _report_scheduled = false;    
    stop_timers();
    if(observed) {
        // initializes and sends an update if observing is on, don't change observing state
        // allows cancel to turn off observing and update state without sending a notification.
        if (check_threshold_values()) {
            handle_timers();
        }
        else {
            _value_not_in_range = true;
            tr_debug("M2MReportHandler::set_under_observation - current value doesn't meet treshold limits");
        }
    }
    else {        
        set_default_values();
    }
}

void M2MReportHandler::set_value(float value)
{
    tr_debug("M2MReportHandler::set_value()");
    _current_value = value;
    if(_current_value != _last_value && _under_observation) {
        _last_value = _current_value;
        if (check_threshold_values()) {
            schedule_report(_current_value);
        }
        else {
            tr_debug("M2MReportHandler::set_value - value not in range");
            if ((_attribute_state & M2MReportHandler::Lt) == M2MReportHandler::Lt ||
                    (_attribute_state & M2MReportHandler::Gt) == M2MReportHandler::Gt) {
                tr_debug("M2MReportHandler::set_value - clear timers");
                _value_not_in_range = true;
                stop_timers();
            }
        }
    }
}

bool M2MReportHandler::parse_notification_attribute(char *&query,
                                                    M2MBase::BaseType type)
{
    tr_debug("M2MReportHandler::parse_notification_attribute(Query %s, Base type %d)", query, (int)type);
    bool success = false;
    char* sep_pos = strchr(query, '&');
    char* rest = query;
    if( sep_pos != NULL ){
        char query_options[5][20];
        uint8_t num_options = 0;
        while( sep_pos != NULL && num_options < 5){
            size_t len = (size_t)(sep_pos-rest);
            if( len > 19 ){
                len = 19;
            }
            strncpy(query_options[num_options], rest, len);
            sep_pos++;
            query_options[num_options++][len] = '\0';
            rest = sep_pos;
            sep_pos = strchr(rest, '&');
        }
        if( num_options < 5 && strlen(rest) > 0){
            size_t len = (size_t)strlen(rest);
            if( len > 19 ){
                len = 19;
            }
            strncpy(query_options[num_options++], rest, len);
        }
        float pmin = _pmin;
        float pmax = _pmax;
        float lt = _lt;
        float gt = _gt;
        float st = _st;
        for (int option = 0; option < num_options; option++) {
            if(set_notification_attribute(query_options[option],type)) {
                tr_debug("M2MReportHandler::parse_notification_attribute - Set Notification Attribute True");
                success = true;
            }
        }
        if(success) {
             success = check_attribute_validity();
        }
        if(!success) {
            _pmin = pmin;
            _pmax = pmax;
            _st = st;
            _lt = lt;
            _gt = gt;
        }
    }
    else {
        if(set_notification_attribute(query, type)) {
            tr_debug("M2MReportHandler::parse_notification_attribute - Set Notification Attribute True");
            success = true;
        }
    }

    return success;
}

void M2MReportHandler::timer_expired(M2MTimerObserver::Type type)
{
    tr_debug("M2MReportHandler::timer_expired(type %d)", (int)type);
    switch(type) {
        case M2MTimerObserver::PMinTimer: {
            if (_report_scheduled ||
                    ((_attribute_state & M2MReportHandler::Pmin) == M2MReportHandler::Pmin &&
                     (_attribute_state & M2MReportHandler::Pmax) != M2MReportHandler::Pmax)){
                tr_debug("M2MReportHandler::timer_expired - PMinTimer _report_scheduled true");
                _report_scheduled = false;
                _pmax_timer->stop_timer();
                report(_current_value);
            }
            else{
                tr_debug("M2MReportHandler::timer_expired - PMinTimer _pmin_exceeded set true");
                _pmin_exceeded = true;
            }
        }
        break;
        case M2MTimerObserver::PMaxTimer: {
            tr_debug("M2MReportHandler::timer_expired - PMaxTimer");
            _pmax_exceeded = true;
            if (_pmin_exceeded) {
                report(_current_value);
            }
        }
        break;
        default:
            break;
    }
}

bool M2MReportHandler::set_notification_attribute(char* option,
                                                  M2MBase::BaseType type)
{
    tr_debug("M2MReportHandler::set_notification_attribute()");
    bool success = false;
    bool observation = true;
    char attribute[20] = {'\0'};
    char value[20] = {'\0'};

    char* pos = strstr(option, EQUAL.c_str());
    if( pos != NULL ){
        strncpy(attribute, option, (size_t)(pos-option));
        pos++;
        attribute[pos-option] = '\0';
        strncpy(value, pos, 20);
    }else{
        strncpy(attribute, option, (size_t)strlen(option));
    }

    if (strcmp(attribute, PMIN.c_str()) == 0) {
        sscanf(value, "%f", &_pmin);
        success = true;
        _attribute_state |= M2MReportHandler::Pmin;
        tr_debug("M2MReportHandler::set_notification_attribute %s to %f", attribute, _pmin);
    }
    else if(strcmp(attribute, PMAX.c_str()) == 0) {
        sscanf(value, "%f", &_pmax);
        success = true;        
        _attribute_state |= M2MReportHandler::Pmax;
        tr_debug("M2MReportHandler::set_notification_attribute %s to %f", attribute, _pmax);
    }
    else if(strcmp(attribute, GT.c_str()) == 0 &&
            M2MBase::Resource == type){
        sscanf(value, "%f", &_gt);
        success = true;        
        _attribute_state |= M2MReportHandler::Gt;
        tr_debug("M2MReportHandler::set_notification_attribute %s to %f", attribute, _gt);
    }
    else if(strcmp(attribute, LT.c_str()) == 0 &&
            M2MBase::Resource == type){
        sscanf(value, "%f", &_lt);
        success = true;
        _attribute_state |= M2MReportHandler::Lt;
        tr_debug("M2MReportHandler::set_notification_attribute %s to %f", attribute, _lt);
    }
    else if(strcmp(attribute, ST.c_str()) == 0 &&
            M2MBase::Resource == type){
        sscanf(value, "%f", &_st);
        success = true;
        _attribute_state |= M2MReportHandler::St;
        tr_debug("M2MReportHandler::set_notification_attribute %s to %f", attribute, _st);
    }
    else if(strcmp(attribute, CANCEL.c_str()) == 0) {
        observation = false;
        success = true;
        _attribute_state |= M2MReportHandler::Cancel;
    }

    if(success) {
        tr_debug("M2MReportHandler::set_notification_attribute %s - set under observation", attribute);
        set_under_observation(observation);
    }
    return success;
}

void M2MReportHandler::schedule_report(float value)
{
    tr_debug("M2MReportHandler::schedule_report()");
    if(_under_observation) {
        if (_pmin_exceeded) {
            stop_timers();
            tr_debug("M2MReportHandler::schedule_report - report value");
            report(value);
        }
        else if(_value_not_in_range) {
            _report_scheduled = true;
            handle_timers();
        }
        else {
            tr_debug("M2MReportHandler::schedule_report - set true");
            _report_scheduled = true;
        }
    }
}

void M2MReportHandler::report(float value)
{
    tr_debug("M2MReportHandler::report()");
    if(_under_observation) {
        _pmin_exceeded = false;
        _pmax_exceeded = false;
        _value_not_in_range = false;
        _high_step = value + _st;
        _low_step = value - _st;
        _observer.observation_to_be_sent();
    }
    handle_timers();
}

void M2MReportHandler::handle_timers()
{
    if (_attribute_state == 0) {
        tr_debug("M2MReportHandler::handle_timers() - params not set!");
        return;
    }

    uint64_t time_interval = 0;
    if(_pmin > 0) {
        time_interval = (uint64_t)(_pmin * 1000);
        tr_debug("M2MReportHandler::handle_timers() - Start PMinTimer");        
        _pmin_timer->start_timer(time_interval,
                                 M2MTimerObserver::PMinTimer,
                                 true);
    } else {
        tr_debug("M2MReportHandler::handle_timers() - _pmin_exceeded");
        _pmin_exceeded = true;
    }
    if(_pmax > 0) {
        time_interval = (uint64_t)(_pmax * 1000);
        tr_debug("M2MReportHandler::handle_timers() - Start PMaxTimer");        
        _pmax_timer->start_timer(time_interval,
                                 M2MTimerObserver::PMaxTimer,
                                 true);
    }
}

bool M2MReportHandler::check_attribute_validity()
{
    bool success = true;
    if ((_attribute_state & M2MReportHandler::Pmax) == M2MReportHandler::Pmax &&
            ((_pmax >= -1.0f) && (_pmin > _pmax))) {
        success = false;
    }
    float low = _lt + 2 * _st;
    if ((_attribute_state & M2MReportHandler::Gt) == M2MReportHandler::Gt &&
            (low >= _gt)) {
        success = false;
    }
    return success;
}

void M2MReportHandler::stop_timers()
{
    tr_debug("M2MReportHandler::stop_timers()");
    _pmin_exceeded = false;
    _pmax_exceeded = false;
    if (_pmin_timer) {
        _pmin_timer->stop_timer();
    }
    if (_pmax_timer) {
        _pmax_timer->stop_timer();
    }
}

void M2MReportHandler::set_default_values()
{
    _pmax = -1.0f;
    _pmin = 1.0f;
    _gt = 0.0f;
    _lt = 0.0f;
    _st = 0.0f;
    _pmin_exceeded = false;    
    _pmax_exceeded = false;
    _report_scheduled = false;
    _last_value = 0.0f;
    _attribute_state = 0;
}

bool M2MReportHandler::check_threshold_values()
{
    tr_debug("M2MReportHandler::check_threshold_values");
    bool can_send = false;
    if ((_current_value >= _high_step ||
        _current_value <= _low_step)) {
        can_send = true;
        // GT & LT set.
        if ((_attribute_state & (M2MReportHandler::Lt | M2MReportHandler::Gt))
                 == (M2MReportHandler::Lt | M2MReportHandler::Gt)) {
            if (_current_value > _gt || _current_value < _lt) {
                can_send = true;
            }
            else {
                can_send = false;
            }
        }
        // Only LT
        else if ((_attribute_state & M2MReportHandler::Lt) == M2MReportHandler::Lt &&
               (_attribute_state & M2MReportHandler::Gt) == 0 ) {
            if (_current_value < _lt) {
                can_send = true;
            }
            else {
                can_send = false;
            }
        }
        // Only GT
        else if ((_attribute_state & M2MReportHandler::Gt) == M2MReportHandler::Gt &&
               (_attribute_state & M2MReportHandler::Lt) == 0 ) {
            if (_current_value > _gt) {
                can_send = true;
            }
            else {
                can_send = false;
            }
        }
        // GT & LT not set.
        else {
            can_send = true;
        }        
    }
    return can_send;
}
