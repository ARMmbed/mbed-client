/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_RESOURCE_INSTANCE_H
#define M2M_RESOURCE_INSTANCE_H

#include "m2mbase.h"
#include "functionpointer.h"

/**
 *  @brief M2MResourceInstance.
 *  This class is the base class for LWM2M Resources based on which all defined
 *  LWM2M resource model can be created.
 */
typedef FP1<void,void*> execute_callback;

class M2MResourceInstance : public M2MBase {

friend class M2MObjectInstance;
friend class M2MResource;

public:

    /**
     * Enum defining resource type which can be
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
         // so that these objects can be created or
         // deleted only through function provided by M2MObjectInstance.
    /**
     * @brief Constructor for creating resource.
     * @param resource_name, name of the resource.
     * @param resource_type, Resource Type.
     * @param type, Resource Data Type of the object.
     */
    M2MResourceInstance(const String &resource_name,
                        const String &resource_type,
                        M2MResourceInstance::ResourceType type);

    /**
     * @brief Constructor for creating resource.
     * @param resource_name, name of the resource
     * @param resource_type, Resource Type.
     * @param type, Resource Data Type of the object.
     * @param value, Value pointer of the object.
     * @param value_length, Length of the value pointer
     */
    M2MResourceInstance(const String &resource_name,
                        const String &resource_type,
                        M2MResourceInstance::ResourceType type,
                        const uint8_t *value,
                        const uint8_t value_length);

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
     * @brief Parses the received query for notification
     * attribute.
     * @return true if required attributes are present else false.
     */
    virtual bool handle_observation_attribute(char *&query);

    /**
     * @brief Sets the function which should be executed when this
     * resource will receive POST command for this resource.
     * @param callback, Function pointer which needs to be executed.
     */
    virtual void set_execute_function(execute_callback callback);

    /**
     * @brief Sets the value of the given resource.
     * @param value, Pointer to the value to be set on the resource.
     * @param value_length , Length of the value pointer.
     * @return True if successfully set else false.
     */
    virtual bool set_value(const uint8_t *value, const uint32_t value_length);

    /**
     * @brief Executes the function which is set in "set_execute_function".
     * @param arguments, arguments that will be passed to execute which
     * needs to be executed.
     */
    void execute(void *arguments);

    /**
     * @brief Provides the value of the given resource.
     * @param value[OUT], pointer to the value of resource.
     * @param value_length[OUT], length to the value pointer.
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

private:

    execute_callback        _execute_callback;
    uint8_t                *_value;
    uint32_t                _value_length;
    ResourceType            _resource_type;

    friend class Test_M2MResourceInstance;
    friend class Test_M2MObjectInstance;
    friend class Test_M2MObject;
    friend class Test_M2MDevice;
    friend class Test_M2MSecurity;
    friend class Test_M2MServer;
    friend class Test_M2MNsdlInterface;
};

#endif // M2M_RESOURCE_INSTANCE_H
