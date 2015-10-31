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
#ifndef M2M_OBJECT_H
#define M2M_OBJECT_H

#include "mbed-client/m2mvector.h"
#include "mbed-client/m2mbase.h"
#include "mbed-client/m2mobjectinstance.h"

//FORWARD DECLARATION
typedef Vector<M2MObjectInstance *> M2MObjectInstanceList;

/**
 *  @brief M2MObject.
 *  This class is the base class for mbed Client Objects based on which all defined
 *  LWM2M objects model can be created. This class will also hold all object
 *  instances associated with the given object.
 */

class M2MObject : public M2MBase,
                  public M2MObjectCallback
{

friend class M2MInterfaceFactory;

protected :

    /**
     * @brief Constructor
     * @param name, name of the object
     */
    M2MObject(const String &object_name);

    // Prevents the use of default constructor.
    M2MObject();

    // Prevents the use of assignment operator.
    M2MObject& operator=( const M2MObject& /*other*/ );

    // Prevents the use of copy constructor
    M2MObject( const M2MObject& /*other*/ );

public:

    /**
     * @brief Destructor
     */
    virtual ~M2MObject();

    /**
     * @brief Creates new object instance for a given object mbed Client Inteface using which
     * client can respond to server's GET methods with the provided value.
     * @return M2MObjectInstance, Object instance to manage other client operations.
     */
    M2MObjectInstance* create_object_instance(uint16_t instance_id = 0);

    /**
     * @brief Removes the object instance resource with given instance id.
     * @param instance_id, Instance ID of object instance to be removed, default is 0.
     * @return True if removed else false.
     */
    virtual bool remove_object_instance(uint16_t instance_id = 0);

    /**
     * @brief Returns object instance with the given instance id.
     * @param instance_id, Instance ID of the requested object instance id, default is 0.
     * @return Object instance reference if found else NULL.
     */
    virtual M2MObjectInstance* object_instance(uint16_t instance_id = 0) const;

    /**
     * @brief Returns list of object instances.
     * @return List of instances with the object.
     */
    virtual const M2MObjectInstanceList& instances() const;

    /**
     * @brief Returns total number of instances with the object.
     * @return Total count of the object instances.
     */
    virtual uint16_t instance_count() const;

    /**
     * @brief Returns object type.
     * @return BaseType.
     */
    M2MBase::BaseType base_type() const;

    /**
     * @brief Adds the observation level for the object.
     * @param observation_level, Level of the observation.
     */
    virtual void add_observation_level(M2MBase::Observation observation_level);

    /**
     * @brief Removes the observation level for the object.
     * @param observation_level, Level of the observation.
     */
    virtual void remove_observation_level(M2MBase::Observation observation_level);

    /**
     * @brief Handles GET request for the registered objects.
     * @param nsdl, NSDL handler for the Coap library.
     * @param received_coap_header, Received CoAP message from the server.
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_get_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);
    /**
     * @brief Handles PUT request for the registered objects.
     * @param nsdl, NSDL handler for the Coap library.
     * @param received_coap_header, Received CoAP message from the server.
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_put_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);

    /**
     * @brief Handles GET request for the registered objects.
     * @param nsdl, NSDL handler for the Coap library.
     * @param received_coap_header, Received CoAP message from the server.
     * @param observation_handler, Handler object for sending
     * observation callbacks.
     * @return sn_coap_hdr_s,  Message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_post_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler = NULL);

protected :

     virtual void notification_update();

private:

    M2MObjectInstanceList     _instance_list; // owned    
    uint16_t                  _max_instance_count;

friend class Test_M2MObject;
friend class Test_M2MInterfaceImpl;
friend class Test_M2MNsdlInterface;
friend class Test_M2MTLVSerializer;
friend class Test_M2MTLVDeserializer;

};

#endif // M2M_OBJECT_H
