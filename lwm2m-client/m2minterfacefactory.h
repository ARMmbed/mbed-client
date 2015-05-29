/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_INTERFACE_FACTORY_H
#define M2M_INTERFACE_FACTORY_H

#include <stdlib.h>
#include "m2msecurity.h"
#include "m2mresource.h"
#include "m2minterfaceobserver.h"

//FORWARD DECLARATION
class M2MDevice;
class M2MServer;
class M2MInterfaceImpl;

/**
 *  @brief M2MInterfaceFactory.
 *  This is a factory class provides interface for creating LWM2M Interface
 *  object for application to utilize LWM2M features provided by the client.
 */

class  M2MInterfaceFactory {
private:
    // Prevents the use of assignment operator by accident.
    M2MInterfaceFactory& operator=( const M2MInterfaceFactory& /*other*/ );

    // Prevents the use of copy constructor by accident
    M2MInterfaceFactory( const M2MInterfaceFactory& /*other*/ );

public:

    /**
     * @brief Creates interface object for LWM2M Inteface using which
     * client can handle LWM2M operations like Bootstrapping, Client
     * Registration, Device Management and Information Reporting.
     * @param endpoint_name, Endpoint name of the mbed client.
     * @param endpoint_type, Endpoint type of the mbed client.
     * @param life_time, Lifetime of the endpoint in seconds,
     *        if -1 then it is optional
     * @param listen_port, Listening port for the endpoint, default is 8000.
     * @param domain, Domain of the endpoint.
     * @param mode, Binding Mode of the endpoint, default is UDP
     * @param stack, Select the underlying network stack to be used for connection.
     * @param context_address, Context address for M2M-HTTP, not used currently.
     * @return M2MInterfaceImpl , Object to manage other LWM2M operations.
     */
    static M2MInterface *create_interface(M2MInterfaceObserver &observer,
                                              const String &endpoint_name,
                                              const String &endpoint_type,
                                              const int32_t life_time = -1,
                                              const uint16_t listen_port = 5683,
                                              const String &domain = "",
                                              M2MInterface::BindingMode mode = M2MInterface::UDP,
                                              M2MInterface::NetworkStack stack = M2MInterface::LwIP_IPv4,
                                              const String &context_address = "");

    /**
     * @brief Creates security object for LWM2M Inteface using which
     * client can manage LWM2M operations bootstrapping and Client Registration.
     * @param ServerType, Type of Security Object whether bootstrap or LWM2M server.
     * @return M2MSecurity, Object to manage other LWM2M operations.
     */
    static M2MSecurity *create_security(M2MSecurity::ServerType server_type);

    /**
     * @brief Creates server object for LWM2M Inteface using which
     * client can manage server resources used for LWM2M operations
     * like Client Registration, server lifetime etc.
     * @return M2MServer, Object to manage server LWM2M operations.
     */
    static M2MServer *create_server();

    /**
     * @brief Creates device object for LWM2M Inteface using which
     * client can manage device resources used for LWM2M operations
     * like Client Registration, Device Management and Information Reporting.
     * @param name, Name of the device object
     * @return M2MDevice, Object to manage other LWM2M operations.
     */
    static M2MDevice *create_device();

    /**
     * @brief Creates generic object for LWM2M Inteface using which
     * client can manage its own customized resources used for registering
     * device Management and Information Reporting for those resources.
     * @param name, Name of the object
     * @param id, Unique ID for the object, it should be other than reserved
     * LWM2M object IDs.
     * @return M2MObject, Object to manage other LWM2M operations.
     */
    static M2MObject *create_object(const String &name);


    friend class Test_M2MInterfaceFactory;
};

#endif // M2M_INTERFACE_FACTORY_H
