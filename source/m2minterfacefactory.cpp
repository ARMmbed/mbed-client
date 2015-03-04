#include "m2minterfacefactory.h"
#include "m2mserver.h"
#include "m2mdevice.h"
#include "m2mobject.h"
#include "m2mconstants.h"

M2MInterfaceImpl* M2MInterfaceFactory::create_interface(M2MInterfaceObserver &observer,
                                                    const String &endpoint_name,
                                                    const String &endpoint_type,
                                                    const uint32_t life_time,
                                                    const String &domain,
                                                    M2MInterface::BindingMode mode,
                                                    M2MInterface::NetworkStack stack,
                                                    const String &context_address)
{
    M2MInterfaceImpl *interface = new M2MInterfaceImpl(observer, endpoint_name,
                                                       endpoint_type, life_time,
                                                       domain, mode,
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
