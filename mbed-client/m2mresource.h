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
#ifndef M2M_RESOURCE_H
#define M2M_RESOURCE_H

#include "mbed-client/m2mvector.h"
#include "mbed-client/m2mresourceinstance.h"

//FORWARD DECLARATION
class M2MObjectInstance;
typedef Vector<M2MResourceInstance *> M2MResourceInstanceList;

class M2MResourceCallback {
public:
    virtual void notification_update() = 0;
};

/*! \file m2mresource.h
 *  \brief M2MResource.
 *  This class is the base class for mbed Client Resources. All defined
 *  LWM2M object models can be created using it. This class will also hold all resources
 *  instances associated with the given object.
 */

class M2MResource : public M2MResourceInstance, M2MResourceCallback {

    friend class M2MObjectInstance;

public:
    class M2MExecuteParameter;

private: // Constructor and destructor are private,
         // which means that these objects can be created or
         // deleted only through a function provided by the M2MObjectInstance.

    M2MResource(M2MObjectInstance &_parent,
                 const lwm2m_parameters_s* s,
                 M2MResourceInstance::ResourceType type,
                 const uint16_t object_instance_id);
    /**
     * \brief Constructor
     * \param resource_name The resource name of the object.
     * \param resource_type The resource type of the object.
     * \param type The resource data type of the object.
     * \param value The value pointer of the object.
     * \param value_length The length of the value pointer.
     * \param path Full path of the resource, eg. 1/2/3. Ownership of the memory is transferred.
     * \param object_instance_id The instance ID of the object where the resource exists.
     * \param object_name The name of the object where the resource exists.
     * \param multiple_instance True if the resource supports instances.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     */
    M2MResource(M2MObjectInstance &_parent,
                const String &resource_name,
                const String &resource_type,
                M2MResourceInstance::ResourceType type,
                const uint8_t *value,
                const uint8_t value_length,
                char *path,
                const uint16_t object_instance_id = 0,
                bool multiple_instance = false,
                bool external_blockwise_store = false);

    /**
     * \brief Constructor
     * \param resource_name The resource name of the object.
     * \param resource_type The resource type of the object.
     * \param type The resource data type of the object.
     * \param observable Indicates whether the resource is observable or not.
     * \param path Full path of the resource, eg. 1/2/3. Ownership of the memory is transferred.
     * \param object_instance_id The ID of the object instance where the resource exists.
     * \param object_name The name of the object where the resource exists.
     * \param multiple_instance True if the resource supports instances.
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     */
    M2MResource(M2MObjectInstance &_parent,
                const String &resource_name,
                const String &resource_type,
                M2MResourceInstance::ResourceType type,
                bool observable,
                char *path,
                const uint16_t object_instance_id = 0,
                bool multiple_instance = false,
                bool external_blockwise_store = false);

    // Prevents the use of a default constructor.
    M2MResource();

    // Prevents the use of an assignment operator.
    M2MResource& operator=( const M2MResource& /*other*/ );

    // Prevents the use of a copy constructor
    M2MResource( const M2MResource& /*other*/ );

    /**
     * Destructor
     */
    virtual ~M2MResource();

public:

    /**
     * \brief Adds resource instances to a M2MResource.
     * \param resource_instance The resource instance to be added.
     */
    void add_resource_instance(M2MResourceInstance *resource_instance);

    /**
     * \brief Returns whether the resource has multiple
     * resource instances or not.
     * \return True if the resource base has multiple instances,
     * else false.
     */
    bool supports_multiple_instances() const;

    /**
     * \brief Sets whether the resource should send a delayed response for a POST request.
     * \param delayed_response A boolean value to set the delayed response.
     */
    void set_delayed_response(bool delayed_response);

    /**
     * \brief A trigger to send the delayed response for the POST request.
     * The delayed_response flag must be set before receiving the POST request
     * and the value of the resource must be updated before calling this function.
     * \return True if a response is sent, else false.
     */
    bool send_delayed_post_response();

    /**
     * \brief Provides the value of the given token.
     * \param value[OUT] A pointer to the token value.
     * \param value_length[OUT] The length of the token pointer.
     */
    void get_delayed_token(uint8_t *&token, uint8_t &token_length);

    /**
     * \brief Removes a resource with a given name.
     * \param name The name of the resource to be removed.
     * \param instance_id The instance ID of the resource to be removed, default is 0.
     * \return True if removed, else false.
     */
    virtual bool remove_resource_instance(uint16_t instance_id = 0);

    /**
     * \brief Returns a resource instance with a given name.
     * \param instance_id The instance ID of the requested resource, default is 0
     * \return M2MResourceInstance object if found, else NULL.
     */
    virtual M2MResourceInstance* resource_instance(uint16_t instance_id = 0) const;

    /**
     * \brief Returns a list of resources.
     * \return A list of resources.
     */
    virtual const M2MResourceInstanceList& resource_instances() const;

    /**
     * \brief Returns the total number of resources.
     * \return The total number of resources.
     */
    virtual uint16_t resource_instance_count() const;

    /**
     * \brief Returns the value set for delayed response.
     * \return The value for delayed response.
     */
    bool delayed_response() const;

    /**
     * \brief Parses the received query for a notification
     * attribute.
     * \return True if required attributes are present, else false.
     */
    virtual bool handle_observation_attribute(const char *query);

    /**
     * \brief Adds the observation level for the object.
     * \param observation_level The level of observation.
     */
    virtual void add_observation_level(M2MBase::Observation observation_level);

    /**
     * \brief Removes the observation level from an object.
     * \param observation_level The level of observation.
     */
    virtual void remove_observation_level(M2MBase::Observation observation_level);

    /**
     * \brief Handles the GET request for registered objects.
     * \param nsdl An NSDL handler for the CoAP library.
     * \param received_coap_header The CoAP message received from the server.
     * \param observation_handler A handler object for sending
     * observation callbacks.
     * \return sn_coap_hdr_s The message that needs to be sent to the server.
     */
    virtual sn_coap_hdr_s* handle_get_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);
    /**
     * \brief Handles the PUT request for registered objects.
     * \param nsdl An NSDL handler for the CoAP library.
     * \param received_coap_header The CoAP message received from the server.
     * \param observation_handler A handler object for sending
     * observation callbacks.
     * \param execute_value_updated True executes the "value_updated" callback.
     * \return sn_coap_hdr_s The message that needs to be sent to the server.
     */
    virtual sn_coap_hdr_s* handle_put_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler,
                                              bool &execute_value_updated);
    /**
     * \brief Handles the POST request for registered objects.
     * \param nsdl An NSDL handler for the CoAP library.
     * \param received_coap_header The CoAP message received from the server.
     * \param observation_handler A handler object for sending
     * observation callbacks.
     * \param execute_value_updated True executes the "value_updated" callback.
     * \return sn_coap_hdr_s The message that needs to be sent to the server.
     */
    virtual sn_coap_hdr_s* handle_post_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler,
                                               bool &execute_value_updated,
                                               sn_nsdl_addr_s *address = NULL);

    M2MObjectInstance& get_parent_object_instance() const;

    /**
     * \brief Returns the name of the object where the resource exists.
     * \return Object name.
    */
    virtual const char* object_name() const;

protected:
    virtual void notification_update();


private:
    M2MObjectInstance &_parent;

    M2MResourceInstanceList     _resource_instance_list; // owned
    uint8_t                     *_delayed_token;
    uint8_t                     _delayed_token_len;
    bool                        _has_multiple_instances;
    bool                        _delayed_response;

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
friend class Test_M2MBase;
friend class Test_M2MResourceInstance;
};

/**
 *  \brief M2MResource::M2MExecuteParameter.
 *  This class handles the "Execute" operation arguments.
 */
class M2MResource::M2MExecuteParameter {

private:

    /**
     * \brief Constructor
     */
    M2MExecuteParameter();

    /**
     * Destructor
     */
   ~M2MExecuteParameter();

public:

    /**
     * \brief Returns the value of an argument.
     * \return uint8_t * The argument value.
     */
    uint8_t *get_argument_value() const;

    /**
     * \brief Returns the length of the value argument.
     * \return uint8_t The argument value length.
     */
    uint16_t get_argument_value_length() const;

    /**
     * \brief Returns the name of the object where the resource exists.
     * \return Object name.
    */
    const String& get_argument_object_name() const;

    /**
     * \brief Returns the resource name.
     * \return Resource name.
    */
    const String& get_argument_resource_name() const;

    /**
     * \brief Returns the instance ID of the object where the resource exists.
     * \return Object instance ID.
    */
    uint16_t get_argument_object_instance_id() const;

private:

    String      _object_name;
    String      _resource_name;
    uint8_t *   _value;
    uint16_t    _value_length;
    uint16_t    _object_instance_id;


friend class Test_M2MResource;
friend class M2MResource;
};

#endif // M2M_RESOURCE_H
