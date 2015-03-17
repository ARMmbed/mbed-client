/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mserver.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mconstants.h"
#include "include/m2minterfaceimpl.h"

M2MInterface* M2MInterfaceFactory::create_interface(M2MInterfaceObserver &observer,
                                                    const String &endpoint_name,
                                                    const String &endpoint_type,
                                                    const uint32_t life_time,
                                                    const uint16_t listen_port,
                                                    const String &domain,
                                                    M2MInterface::BindingMode mode,
                                                    M2MInterface::NetworkStack stack,
                                                    const String &context_address)
{
    M2MInterfaceImpl *interface = new M2MInterfaceImpl(observer, endpoint_name,
                                                   endpoint_type, life_time,
                                                   listen_port, domain, mode,
                                                   stack, context_address);
    return interface;
}

M2MSecurity* M2MInterfaceFactory::create_security(M2MSecurity::ServerType server_type)
{
    M2MSecurity *security = new M2MSecurity(server_type);
    return security;
}

M2MServer* M2MInterfaceFactory::create_server()
{
    M2MServer *server = new M2MServer();
    return server;
}

M2MDevice* M2MInterfaceFactory::create_device()
{
    M2MDevice* device = new M2MDevice();
    return device;
}

M2MObject* M2MInterfaceFactory::create_object(const String &name)
{
    M2MObject *object = NULL;
    object = new M2MObject(name);
    return object;
}
