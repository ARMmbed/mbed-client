/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mserver.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mconstants.h"
#include "lwm2m-client/m2mconfig.h"
#include "include/m2minterfaceimpl.h"
#include "ns_trace.h"

M2MInterface* M2MInterfaceFactory::create_interface(M2MInterfaceObserver &observer,
                                                    const String &endpoint_name,
                                                    const String &endpoint_type,
                                                    const int32_t life_time,
                                                    const uint16_t listen_port,
                                                    const String &domain,
                                                    M2MInterface::BindingMode mode,
                                                    M2MInterface::NetworkStack stack,
                                                    const String &context_address)
{
    tr_debug("M2MInterfaceFactory::create_interface - IN");
    tr_debug("M2MInterfaceFactory::create_interface - parameters endpoint name : %s",endpoint_name.c_str());
    tr_debug("M2MInterfaceFactory::create_interface - parameters endpoint type : %s",endpoint_type.c_str());
    tr_debug("M2MInterfaceFactory::create_interface - parameters life time(in secs):  %d",life_time);
    tr_debug("M2MInterfaceFactory::create_interface - parameters Listen Port : %d",listen_port);
    tr_debug("M2MInterfaceFactory::create_interface - parameters Binding Mode : %d",(int)mode);
    tr_debug("M2MInterfaceFactory::create_interface - parameters NetworkStack : %d",(int)stack);
    M2MInterfaceImpl *interface = NULL;
    if( ((life_time == -1) || (life_time >= MINIMUM_REGISTRATION_TIME)) &&
        !endpoint_name.empty() && (endpoint_name.size() <= ENDPOINT_LENGTH) &&
        !endpoint_type.empty() && (endpoint_type.size() <= ENDPOINT_LENGTH)) {
        tr_debug("M2MInterfaceFactory::create_interface - Creating M2MInterfaceImpl");
        interface = new M2MInterfaceImpl(observer, endpoint_name,
                                         endpoint_type, life_time,
                                         listen_port, domain, mode,
                                         stack, context_address);

    }
    tr_debug("M2MInterfaceFactory::create_interface - OUT");
    return interface;
}

M2MSecurity* M2MInterfaceFactory::create_security(M2MSecurity::ServerType server_type)
{
    tr_debug("M2MInterfaceFactory::create_security");
    M2MSecurity *security = new M2MSecurity(server_type);
    return security;
}

M2MServer* M2MInterfaceFactory::create_server()
{
    tr_debug("M2MInterfaceFactory::create_server");
    M2MServer *server = new M2MServer();
    return server;
}

M2MDevice* M2MInterfaceFactory::create_device()
{
    tr_debug("M2MInterfaceFactory::create_device");
    M2MDevice* device = new M2MDevice();
    return device;
}

M2MObject* M2MInterfaceFactory::create_object(const String &name)
{
    tr_debug("M2MInterfaceFactory::create_object : Name : %s", name.c_str());
    M2MObject *object = NULL;
    object = new M2MObject(name);
    return object;
}
