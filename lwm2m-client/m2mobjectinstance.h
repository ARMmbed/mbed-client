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
#ifndef M2M_OBJECT_INSTANCE_H
#define M2M_OBJECT_INSTANCE_H

#include "m2mvector.h"
#include "m2mresource.h"

//FORWARD DECLARATION
typedef Vector<M2MResource *> M2MResourceList;
typedef Vector<M2MResourceInstance *> M2MResourceInstanceList;

/**
 *  @brief M2MObjectInstance.
 *  This class is the instance class for LWM2M Objects based on which all defined
 *  LWM2M objects model can be created. This class will also hold all resources
 *  instances associated with the given object.
 */

class M2MObjectInstance : public M2MBase {

friend class M2MObject;

private: // Constructor and destructor are private
         // so that these objects can be created or
         // deleted only through function provided by M2MObject.

    /**
     * @brief Constructor
     * @param name, name of the object
     */
    M2MObjectInstance(const String &object_name);


    // Prevents the use of default constructor.
    M2MObjectInstance();

    // Prevents the use of assignment operator.
    M2MObjectInstance& operator=( const M2MObjectInstance& /*other*/ );

    // Prevents the use of copy constructor
    M2MObjectInstance( const M2MObjectInstance& /*other*/ );

    /**
     * Destructor
     */
    virtual ~M2MObjectInstance();

public:

    /**
     * @brief Creates static resource for a given object LWM2M Inteface using which
     * client can respond to server's GET methods with the provided value.
     * @param resource_name, Name of the resource.
     * @param resource_type, Type of the resource.
     * @param value, pointer to the value of the resource.
     * @param value_length, length to the value in pointer.
     * @param multiple_instance, Resource can have
     *        multiple instances,default is false.
     * @return M2MResource, Resource to manage other LWM2M operations.
     */
    M2MResource* create_static_resource(const String &resource_name,
                                        const String &resource_type,
                                        M2MResourceInstance::ResourceType type,
                                        const uint8_t *value,
                                        const uint8_t value_length,
                                        bool multiple_instance = false);

    /**
     * @brief Creates dynamic resource for a given object LWM2M Inteface using which
     * client can respond to different queries from server (GET,PUT etc).Also,
     * this type of resource is observable and will carry callbacks.
     * @param resource_name, Name of the resource.
     * @param resource_type, Type of the resource.
     * @param observable, whether resource is observable or not.
     * @param multiple_instance, Resource can have
     *        multiple instances, default is false.
     * @return M2MResource, Resource to manage other LWM2M operations.
     */
    M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         bool observable,
                                         bool multiple_instance = false);


    /**
     * @brief Creates static resource instance for a given object LWM2M Inteface using which
     * client can respond to server's GET methods with the provided value.
     * @param resource_name, Name of the resource.
     * @param resource_type, Type of the resource.
     * @param value, pointer to the value of the resource.
     * @param value_length, length to the value in pointer.
     * @param instance_id, Instance Id of the resource.
     * @return M2MResourceInstance, Resource Instance to manage other LWM2M operations.
     */
    M2MResourceInstance* create_static_resource_instance(const String &resource_name,
                                                         const String &resource_type,
                                                         M2MResourceInstance::ResourceType type,
                                                         const uint8_t *value,
                                                         const uint8_t value_length,
                                                         uint16_t instance_id);

    /**
     * @brief Creates dynamic resource instance for a given object LWM2M Inteface using which
     * client can respond to different queries from server (GET,PUT etc).Also,
     * this type of resource is observable and will carry callbacks.
     * @param resource_name, Name of the resource.
     * @param resource_type, Type of the resource.
     * @param observable, whether resource is observable or not.
     * @param instance_id, Instance Id of the resource.
     * @return M2MResourceInstance, Resource Instance to manage other LWM2M operations.
     */
    M2MResourceInstance* create_dynamic_resource_instance(const String &resource_name,
                                                          const String &resource_type,
                                                          M2MResourceInstance::ResourceType type,
                                                          bool observable,
                                                          uint16_t instance_id);

    /**
     * @brief Removes the resource with given name.
     * @param name, Name of the resource to be removed.
     * @return True if removed else false.
     */
    virtual bool remove_resource(const String &name);

    /**
     * @brief Removes the resource instance with given name.
     * @param resource_name, Name of the resource instance to be removed.
     * @param instance_id, Instance Id of the instance.
     * @return True if removed else false.
     */
    virtual bool remove_resource_instance(const String &resource_name,
                                          uint16_t instance_id);

    /**
     * @brief Returns resource with the given name.
     * @param name, Name of the requested resource.
     * @return Resource reference if found else NULL.
     */
    virtual M2MResource* resource(const String &name) const;

    /**
     * @brief Returns list of M2MResourceBase objects.
     * @return List of Resource base with the object instance.
     */
    virtual const M2MResourceList& resources() const;

    /**
     * @brief Returns total number of resources with the object.
     * @return Total count of the resources.
     */
    virtual uint16_t resource_count() const;

    /**
     * @brief Returns total instance of single resource.
     * @param resource, Name of the resource.
     * @return Total count of the resources.
     */
    virtual uint16_t resource_count(const String& resource) const;

    /**
     * @brief Returns object type.
     * @return BaseType.
     */
    M2MBase::BaseType base_type() const;

    /**
     * @brief Parses the received query for notification
     * attribute.
     * @return true if required attributes are present else false.
     */
    virtual bool handle_observation_attribute(char *&query);

    /**
     * @brief Handles GET request for the registered objects.
     * @param nsdl, NSDL handler for the Coap library
     * @param received_coap_header, Received CoAP message from the server
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_get_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);
    /**
     * @brief Handles PUT request for the registered objects.
     * @param nsdl, NSDL handler for the Coap library
     * @param received_coap_header, Received CoAP message from the server
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_put_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);

    /**
     * @brief Handles GET request for the registered objects.
     * @param nsdl, NSDL handler for the Coap library
     * @param received_coap_header, Received CoAP message from the server
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_post_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler = NULL);

private:

    M2MResourceList     _resource_list; // owned

    friend class Test_M2MObjectInstance;
    friend class Test_M2MObject;
    friend class Test_M2MDevice;
    friend class Test_M2MSecurity;
    friend class Test_M2MServer;
    friend class Test_M2MNsdlInterface;
};

#endif // M2M_OBJECT_INSTANCE_H
