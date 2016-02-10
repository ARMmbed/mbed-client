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
#ifndef M2M_RESOURCE_INSTANCE_H
#define M2M_RESOURCE_INSTANCE_H

#include "mbed-client/m2mbase.h"
#include "mbed-client/functionpointer.h"

class M2MObjectInstanceCallback {
public:
    virtual void notification_update(M2MBase::Observation observation_level) = 0;
};

/**
 *  @brief M2MResourceInstance.
 *  This class is the base class for mbed Client Resources. All defined
 *  LWM2M resource models can be created based on it.
 */
typedef FP1<void,void*> execute_callback;
class M2MResourceCallback;

class M2MResourceInstance : public M2MBase {

friend class M2MObjectInstance;
friend class M2MResource;

public:

    /**
     * Enum defining a resource type that can be
     * supported by a given resource.
    */
    typedef enum {
        STRING,
        INTEGER,
        FLOAT,
        BOOLEAN,
        OPAQUE,
        TIME,
        OBJLINK
    }ResourceType;


private: // Constructor and destructor are private
         // which means that these objects can be created or
         // deleted only through a function provided by the M2MObjectInstance.
    /**
     * @brief Constructor for creating resource.
     * @param resource_name, Name of the resource.
     * @param resource_type, Resource Type.
     * @param type, Resource Data Type of the object.
     */
    M2MResourceInstance(const String &resource_name,
                        const String &resource_type,
                        M2MResourceInstance::ResourceType type,
                        M2MObjectInstanceCallback &object_instance_callback);

    /**
     * @brief Constructor for creating a resource.
     * @param resource_name, Name of the resource.
     * @param resource_type, Resource Type.
     * @param type, Resource Data Type of the object.
     * @param value, Value pointer of the object.
     * @param value_length, Length of the value pointer.
     */
    M2MResourceInstance(const String &resource_name,
                        const String &resource_type,
                        M2MResourceInstance::ResourceType type,
                        const uint8_t *value,
                        const uint8_t value_length,
                        M2MObjectInstanceCallback &object_instance_callback);

    // Prevents the use of default constructor.
    M2MResourceInstance();

    // Prevents the use of assignment operator.
    M2MResourceInstance& operator=( const M2MResourceInstance& /*other*/ );

    // Prevents the use of copy constructor
    M2MResourceInstance( const M2MResourceInstance& /*other*/ );

    /**
     * Destructor
     */
    virtual ~M2MResourceInstance();

public:

    /**
     * @brief Returns object type.
     * @return BaseType.
     */
    virtual M2MBase::BaseType base_type() const;

    /**
     * @brief Returns resource data type.
     * @return ResourceType.
     */
    virtual M2MResourceInstance::ResourceType resource_instance_type() const;

    /**
     * @brief Parses the received query for a notification
     * attribute.
     * @return True if required attributes are present, else false.
     */
    virtual bool handle_observation_attribute(char *&query);

    /**
     * @brief Sets the function that should be executed when this
     * resource receives a POST command.
     * @param callback, Function pointer that needs to be executed.
     */
    virtual void set_execute_function(execute_callback callback);

    /**
     * @brief Sets the value of the given resource.
     * @param value, Pointer to the value to be set on the resource.
     * @param value_length , Length of the value pointer.
     * @return True if successfully set, else false.
     */
    virtual bool set_value(const uint8_t *value, const uint32_t value_length);

    /**
     * @brief Clears the value of the given resource.
     */
    virtual void clear_value();

    /**
     * @brief Executes the function that is set in "set_execute_function".
     * @param arguments, Arguments that will be passed to be executed.
     */
    void execute(void *arguments);

    /**
     * @brief Provides the value of the given resource.
     * @param value[OUT], Pointer to the resource value.
     * @param value_length[OUT], Length of the value pointer.
     */
    virtual void get_value(uint8_t *&value, uint32_t &value_length);

    /**
     * @brief Returns value pointer of the object.
     * @return Value pointer of the object.
    */
    uint8_t* value() const;

    /**
     * @brief Returns length of the value pointer.
     * @return Length of the value pointer.
    */
    uint32_t value_length() const;

    /**
     * @brief Handles GET request for the registered objects.
     * @param nsdl, NSDL handler for the CoAP library.
     * @param received_coap_header, CoAP message received from the server.
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to the server.
     */
    virtual sn_coap_hdr_s* handle_get_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);
    /**
     * @brief Handles PUT request for the registered objects.
     * @param nsdl, NSDL handler for the CoAP library.
     * @param received_coap_header, CoAP message received from the server.
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @param execute_value_updated, True will execute "value_updated" callback.
     * @return sn_coap_hdr_s,  Message that needs to be sent to the server.
     */
    virtual sn_coap_hdr_s* handle_put_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,                                                                                            
                                              M2MObservationHandler *observation_handler,
                                              bool &execute_value_updated);

protected:

    /**
     * @brief Set observer for sending notification update.
     * @param resource, callback handle.
     */
    void set_resource_observer(M2MResourceCallback *resource);

private:

    void report();

    bool is_value_changed(const uint8_t* value, const uint32_t value_len);

private:

    M2MObjectInstanceCallback               &_object_instance_callback;
    execute_callback                        _execute_callback;
    uint8_t                                 *_value;
    uint32_t                                _value_length;
    ResourceType                            _resource_type;
    M2MResourceCallback                     *_resource_callback; // Not owned

    friend class Test_M2MResourceInstance;
    friend class Test_M2MResource;
    friend class Test_M2MObjectInstance;
    friend class Test_M2MObject;
    friend class Test_M2MDevice;
    friend class Test_M2MSecurity;
    friend class Test_M2MServer;
    friend class Test_M2MNsdlInterface;
    friend class Test_M2MFirmware;
    friend class Test_M2MTLVSerializer;
    friend class Test_M2MTLVDeserializer;
};

#endif // M2M_RESOURCE_INSTANCE_H
