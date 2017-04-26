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
#ifndef M2MREPORTHANDLER_H
#define M2MREPORTHANDLER_H

// Support for std args
#include <stdint.h>
#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2mbase.h"
#include "mbed-client/m2mtimerobserver.h"
#include "mbed-client/m2mresourceinstance.h"
#include "mbed-client/m2mvector.h"
#include "mbed-client/m2mtimer.h"

//FORWARD DECLARATION
class M2MReportObserver;
class M2MTimer;
class M2MResourceInstance;

/**
 *  @brief M2MReportHandler.
 *  This class is handles all the observation related operations.
 */
class M2MReportHandler: public M2MTimerObserver
{
private:
    // Prevents the use of assignment operator by accident.
    M2MReportHandler& operator=( const M2MReportHandler& /*other*/ );

public:

    M2MReportHandler(M2MReportObserver &observer);

public:

    /**
     * Enum defining which write attributes are set.
    */
    enum {
        Cancel = 1,
        Pmin = 2,
        Pmax = 4,
        Lt = 8,
        Gt = 16,
        St = 32
    };

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
    void set_under_observation(bool observed);

    /**
     * @brief Sets the value of the given resource.
     * @param value, Value of the observed resource.
     */
    void set_value(float value);

    /**
     * @brief Sets notification trigger.
     * @param obj_instance_id, Object instance id that has changed
     */
    void set_notification_trigger(uint16_t obj_instance_id = 0);

    /**
     * @brief Parses the received query for notification
     * attribute.
     * @param query Query to be parsed for attributes.
     * @param type Type of the Base Object.
     * @param resource_type Type of the Resource.
     * @return true if required attributes are present else false.
     */
    bool parse_notification_attribute(const char *query,
                                              M2MBase::BaseType type,
                                              M2MResourceInstance::ResourceType resource_type = M2MResourceInstance::OPAQUE);

    /**
    * @brief Set back to default values.
    */
    void set_default_values();

    /**
     * @brief Return write attribute flags.
     */
    uint8_t attribute_flags() const;

    /**
     * \brief Sets the observation token value.
     * \param token A pointer to the token of the resource.
     * \param length The length of the token pointer.
     */
    void set_observation_token(const uint8_t *token, const uint8_t length);

    /**
     * \brief Provides a copy of the observation token of the object.
     * \param value[OUT] A pointer to the value of the token.
     * \param value_length[OUT] The length of the token pointer.
     */
    void get_observation_token(const uint8_t *&token, uint32_t &token_length) const;

    /** Deprecated compatibility wrapper for get_observation_token(const uint8_t*&). */
    void get_observation_token(uint8_t *&token, uint32_t &token_length) const;

    /**
     * \brief Returns the observation number.
     * \return The observation number of the object.
     */
    uint16_t observation_number() const;

    /**
     * \brief Adds the observation level for the object.
     * \param observation_level The level of observation.
     */
    void add_observation_level(M2MBase::Observation obs_level);

    /**
     * \brief Removes the observation level for the object.
     * \param observation_level The level of observation.
     */
    void remove_observation_level(M2MBase::Observation obs_level);

    /**
     * \brief Returns the observation level of the object.
     * \return The observation level of the object.
     */
    M2MBase::Observation observation_level() const;

    /**
     * @brief Returns whether this resource is under observation or not.
     * @return True if the resource is under observation, else false,
     */
    bool is_under_observation() const;

protected : // from M2MTimerObserver

    virtual void timer_expired(M2MTimerObserver::Type type =
                               M2MTimerObserver::Notdefined);

private:



    bool set_notification_attribute(const char* option,
            M2MBase::BaseType type,
            M2MResourceInstance::ResourceType resource_type);

    /**
     * @brief Schedule a report, if the pmin is exceeded
     * then report immediately else store the state to be
     * reported once the time fires.
     */
    void schedule_report();

    /**
    * @brief Reports a sample that satisfies the reporting criteria.
    */
    void report();

    /**
    * @brief Manage timers for pmin and pmax.
    */
    void handle_timers();

    /**
    * @brief Check whether notification params can be accepted.
    */
    bool check_attribute_validity() const;

    /**
    * @brief Stop pmin & pmax timers.
    */
    void stop_timers();

    /**
     * @brief Check if current value match threshold values.
     * @return True if notify can be send otherwise false.
     */
    bool check_threshold_values() const;

    /**
     * @brief Check whether current value matches with GT & LT.
     * @return True if current value match with GT or LT values.
     */
    bool check_gt_lt_params() const;

    /**
     * \brief Allocate (size + 1) amount of memory, copy size bytes into
     * it and add zero termination.
     * \param source The source string to copy, may not be NULL.
     * \param size The size of memory to be reserved.
    */
    static uint8_t* alloc_string_copy(const uint8_t* source, uint32_t size);

private:
    M2MReportObserver           &_observer;
    bool                        _is_under_observation : 1;
    M2MBase::Observation        _observation_level : 4;
    uint8_t                     _attribute_state;
    unsigned                    _token_length : 8;
    bool                        _notify;
    bool                        _pmin_exceeded;
    bool                        _pmax_exceeded;
    unsigned                    _observation_number : 16;
    M2MTimer                    _pmin_timer;
    M2MTimer                    _pmax_timer;
    uint8_t                     *_token;
    int32_t                     _pmax;
    int32_t                     _pmin;
    float                       _current_value;
    float                       _gt;
    float                       _lt;
    float                       _st;
    float                       _high_step;
    float                       _low_step;
    float                       _last_value;
    m2m::Vector<uint16_t>       _changed_instance_ids;

friend class Test_M2MReportHandler;

};

#endif // M2MREPORTHANDLER_H
