/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_SECURITY_H
#define M2M_SECURITY_H

#include "m2mobject.h"

// FORWARD DECLARATION
class M2MResource;

/**
 *  @brief M2MSecurity.
 *  This class represents interface for the Security Object model of LWM2M framework.
 *  This class will handle the security object instances and all its corresponding
 *  resources.
 */

class  M2MSecurity : public M2MObject {

friend class M2MInterfaceFactory;
friend class M2MNsdlInterface;

public:

    /**
     * @brief Enum defining all the resources associdated with
     * Security Object in LWM2M framework.
     */
    typedef enum {
        M2MServerUri,
        BootstrapServer,
        SecurityMode,
        PublicKey,
        ServerPublicKey,
        Secretkey,
        SMSSecurityMode,
        SMSBindingKey,
        SMSBindingSecretKey,
        M2MServerSMSNumber,
        ShortServerID,
        ClientHoldOffTime
    }SecurityResource;

    typedef enum {
        SecurityNotSet,
        Certificate,
        NoSecurity
    } SecurityModeType;
    /**
     * @brief Enum defining interface operation which can be
     * handled by the Security Object.
     */
    typedef enum {
        Bootstrap = 0x0,
        M2MServer = 0x1
    } ServerType;

private:

    /**
     * @brief Constructor
     * @param server_type, Type of security object created whether bootstrap or LWM2M server
     */
    M2MSecurity(ServerType server_type);

    // Prevents the use of default constructor.
    M2MSecurity();

    // Prevents the use of assignment operator.
    M2MSecurity& operator=( const M2MSecurity& /*other*/ );

    // Prevents the use of copy constructor
    M2MSecurity( const M2MSecurity& /*other*/ );

public:

    /**
     * @brief Destructor
     */
    virtual ~M2MSecurity();

    /**
     * @brief Creates a new resource for given resource enum.
     * @param rescource, List of resource names which can be created using this function are
     * ' BootstrapServer', 'SecurityMode', 'SMSSecurityMode',
     * 'M2MServerSMSNumber', 'ShortServerID', 'ClientHoldOffTime'.
     * @param value, Value to be set on the resource, in Integer format
     * @return M2MResource if created successfully else NULL.
     */
    M2MResource* create_resource(SecurityResource rescource, uint32_t value);

    /**
     * @brief Deletes the resource with the given resource enum.
     * It cannot not delete the mandatory resources.
     * @param resource, Resource to be deleted.
     * @return True if deleted else false.
     */
    bool delete_resource(SecurityResource rescource);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource name for which value can be set
     * using this function are
     * 'M2MServerUri', 'SMSBindingKey', 'SMSBindingSecretKey'.
     * @param value, Value to be set on the key, in String format
     * @return True if successfully set else false.
     */
    bool set_resource_value(SecurityResource resource,
                            const String &value);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource name for which value can be set
     * using this function are
     * 'BootstrapServer', 'SecurityMode', 'SMSSecurityMode',
     * 'M2MServerSMSNumber', 'ShortServerID', 'ClientHoldOffTime'.
     * @param value, Value to be set on the resource, in Integer format
     * @return True if successfully set else false.
     */
    bool set_resource_value(SecurityResource resource,
                            uint32_t value);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource name for which value can be set
     * using this function are
     * 'PublicKey', 'ServerPublicKey', 'Secretkey'.
     * @param value, Value to be set on the key, in uint8_t format
     * @param size, size of the buffer value to be set on the key.
     * @return True if successfully set else false.
     */
    bool set_resource_value(SecurityResource resource,
                            const uint8_t *value,
                            const uint16_t length);

    /**
     * @brief Returns the value of the given resource enum, in String
     * @param resource, List of resource names which can return value using this function are
     * 'M2MServerUri','SMSBindingKey', 'SMSBindingSecretKey'.
     * @return Value associated with that resource, if resource is not valid it returns empty string.
     */
    String resource_value_string(SecurityResource resource) const;

    /**
     * @brief Populates the data buffer and returns the size of the buffer.
     * @param resource, List of resource names which can return value using this function are
     * 'PublicKey', 'ServerPublicKey', 'Secretkey'.
     * @param [OUT] data, Data buffer which will contain the value.
     * @return Size of the buffer populated .
     */
    uint32_t resource_value_buffer(SecurityResource resource,
                                   uint8_t *&data) const;

    /**
     * @brief Returns the value of the given resource name, in Integer
     * @param resource, List of resource names which can return value using this function are
     * 'BootstrapServer', 'SecurityMode', 'SMSSecurityMode',
     * 'M2MServerSMSNumber', 'ShortServerID', 'ClientHoldOffTime'.
     * @return Value associated with that resource, if resource is not valid it can returns 0.
     */
    uint32_t resource_value_int(SecurityResource resource) const;


    /**
     * @brief Returns if the resource instance with given resource enum exists or not
     * @param resource, resource enum.
     * @return True if at least one instance exists else false.
     */
    bool is_resource_present(SecurityResource resource)const;

    /**
     * @brief Returns total number of resources for security object.
     * @return Total Number of resources.
     */
    uint16_t total_resource_count()const;

    /**
     * @brief Returns the type of Security Object. It can be either
     * Bootstrap or M2MServer.
     * @return ServerType, Type of the Security Object.
     */
    ServerType server_type() const;

private:

    M2MResource* get_resource(SecurityResource resource) const;

private:

    ServerType            _server_type;
    M2MObjectInstance*    _server_instance;

    friend class Test_M2MSecurity;
    friend class Test_M2MInterfaceImpl;
};

#endif // M2M_SECURITY_H


