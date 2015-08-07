# mbed-Client

## Introduction

mbed Device Client API enables the use of LWM2M features from the mbed Device Client. These features are described in the [Lightweight Machine to Machine Technical Specification](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0). Using this API, the mbed OS developers can develop their own applications utilizing the LWM2M features. 

This API provides an enabler that defines the application layer communication protocol between a mbed Device Server and a client located in a mbed Device. 

The API includes device management and service enablement for mbed Devices. The target mbed devices for this enabler are mainly resource constrained devices. Therefore, this enabler makes use of a light and compact protocol as well as an efficient resource data model.

Client-server architecture is introduced for mbed Device Client. The mbed Device acts as the client, and the M2M service, platform or application acts as the mbed Device Server.

There are four interfaces between these two components as follows:

- Bootstrap
- Client registration
- Device management and service enablement
- Information reporting


##mbed Client features

The API provides the application developer with an interface to define the application endpoint information that will be delivered to the Bootstrap Server and mbed Device Server during the bootstrap and registration operations respectively.

This is how you can create your interface for your endpoint:
```
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterface.h"

M2MInterface* interface = M2MInterfaceFactory::create_interface(*this,
                                                  "mbed-endpoint",
                                                  "test",
                                                  3600,
                                                  8000,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
```
When you have created the interface, you can proceed to execute operations.

### Bootstrap feature

The Bootstrap Interface is used to provision essential information into the client to enable the client to perform the **Register** operation with one or more mbed Device Servers. 

In this release, only the client initiated bootstrap mode is supported.

### Client initiated bootstrap

The client initiated bootstrap mode provides a mechanism for client to retrieve the bootstrap information from a LWM2M Bootstrap Server. This mode requires a LWM2M Boostrap Server account.

You can provide the bootstrap server information and issue the bootstrap command as follows:

First, you need to create your bootstrap server object that contains information about bootstrap server, for example server address and security mode used by the server.

```
#include "mbed-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, BOOTSTRAP_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 1);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
```

**Note**: Currently, this API supports only non-secure mode operations. Security features will be added in upcoming  releases.
 
When you have the bootstrap object ready, you only need to call the bootstrap API by passing this object as parameter.

```
M2MInterface::bootstrap(M2MSecurity* bootstrap_object);
```

Because this is an asynchronous operation, you will receive the result of it through a callback defined in `m2minterfaceobserver.h` that you are handling in your application.

If the bootstrap operation is successful and the client can fetch the mbed Device Server information from the Bootstrap Server, your application will receive the following callback:

```
void bootstrap_done(M2MSecurity *server_object)
```

The `server_object` contains the data for the mbed Device Server, for example, server URI and security mode. With this object, you can execute the registration operation for the client.

If the bootstrap operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback and then you can act accordingly.

### Client Registration Interface

The Client Registration Interface is used by the client to register with mbed Device Servers, maintain registration and de-register from the mbed Device Server.

Currently, only one-to-one client-server registration is supported. One-to-many client-server registrations will be supported in upcoming releases.

The Client Registration Interface includes multiple sub-features. The following are supported currently:

- Register
- Update 
- De-register


### Register

When registering, the client performs the **Register** operation and provides the properties that the mbed Device Server requires to contact the client (for example End Point Name); maintains the registration and session (for example Lifetime, Queue Mode) between the client and mbed Device Server, and information on the Objects the client supports and existing Object Instances in the client.

This API enables the client registration functionality.

You can provide the mbed Device Server information and issue register command as follows:

First you need to create your mbed Device Server object that contains information about the Bootstrap Server, such as server address and security mode used by the server.

If you have performed the bootstrap operation first, this object is automatically created and available through the `bootstrap_done()` callback that you can use directly.

```
#include "mbed-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::LWM2M);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, LWM2M_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 0);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
```

**Note**: This API supports only non-secure mode operations. Security features will be added in upcoming releases.

When registering your endpoint, you will also need to register all your resources that you would like to monitor or follow via the mbed Device Server.

To do this, create the resource objects and pass them to the register API for registration purposes.

For example, if you want to register your OMA LWM2M based Device object, you need to create the device object and set the values for manadatory resources as follows:

```
#include "mbed-client/m2mdevice.h"
M2MDevice *device = M2MInterfaceFactory::create_device();
if(device) {
    device->create_resource(M2MDevice::Manufacturer,MANUFACTURER);
	device->create_resource(M2MDevice::DeviceType,TYPE);
	device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER);
	device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER);
    }
```

You can register other resources as well. Please check the doxygen documentation for the detailed API description.

Apart from the manadatory device obejct, if you would like to register your own customized resources, you can create them and set their values accordingly.

To do that, please check the API documentation for M2MObject, M2MObjectInstance and M2MResource classes.

When you have the registration server object and resources that you want to register, you need to call the register API and pass the following objects as parameters:

```
M2MInterface::register_object(M2MSecurity* register_object, M2MObjectList object_list);
```

Because this is an asynchronous operation, you will receive the result of this operation through a callback defined in `m2minterfaceobserver.h` that you are handling in your application.

If the register operation is successful and the client can register all your resources to the mbed Device Server information, your application will receive the following callback:

```
void object_registered(M2MSecurity *server_object, const M2MServer& server)
```

The `M2MSecurity *server_object` informs to which mbed Device Server the client has just registered and `M2MServer &server` contains the data related to mbed Device Server including for example the Short ServerID and the client registration period.

If the registration operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback and then you can act accordingly.

### Update

Periodically or based on certain events within the client or initiated by the mbed Device Server, the client updates its registration information with the mbed Device Server by sending an **Update** operation to the mbed Device Server.
This is how you can update your registration:

```
M2MInterface::update_registration(M2MSecurity* security_object, const uint32_t lifetime)
```

Normally, the enabler will update the registration automatically but if you want to renew the registration before that, you can use this API.

If the update operation is successful, your application will receive the following callback:

```
void registration_updated(M2MSecurity *const M2MServer& server)
```

The `M2MSecurity *server_object` informs to which mbed Device Server the client has just updated the registration and `M2MServer &server` contains the data related with mbed Device Server including for example the Short ServerID and the client registration period.

If the updation operation fails for some reason,  you will receive the following callback:

```
void error(M2MInterface::Error error)
```

### Unregister

When the client determines that it no longer requires to be available to the mbed Device Server (LWM2M Device factory reset, for example), the client will unregister from the mbed Device Server. Upon receiving this message, the mbed Device Server removes the registration information from the mbed Device Server.

This is how you can unregister your endpoint client:

If the endpoint has multiple server registrations, you need to provide the `server_object` of the server where you would like to unregister your endpoint. Otherwise, if there is only one registration, you can pass `NULL` and the client will unregister the default registration from the endpoint.

```
M2MInterface::unregister_object(M2MSecurity *object);
```

Because this is an asynchronous operation, you will receive the result of this operation through callback defined in `m2minterfaceobserver.h` which you are handling in your application.

If the unregister operation is successful and the client is successfully unregistered from mbed Device Server information, your application will receive the following callback:

```
void object_unregistered(M2MSecurity *server_object)
```

The `M2MSecurity *server_object` informs from which mbed Device Server the client has just unregistered.

If the unregistration operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback and then you can act accordingly.

### Device Management and Service Enabler Interface

The Device Management and Service Enabler Interface is used by the mbed Device Server to access Object Instances and Resources available in the client. The interface provides this access through the following operations:

- **Create**
- **Read**
- **Write**
- **Delete**
- **Execute**
- **Write Attributes**

The Device Management and Service Enabler Interface supports currently the following sub-features:

- Read
- Write
- Write Attributes
- Execute
- Create (For ObjectInstances)
- Delete (For ObjectInstances)

The Device Management and Service Enabler Interface is handled at Resources level for text, support for Object and Object Instances is available for TLV data type.

### Read

The **Read** operation is used to access the value of a Resource, an array of Resource Instances, an Object Instance or all the Object Instances of an Object.

The Client API enables you to set the value of resources that can be read by mbed Device server.

There are two types of resources you can create:

- Static
- Dynamic

In a Static resource, you set the value of the resource once and it does not change during the course of operations. You can use Static resources to report values such as Device name, type and address.

For example, you can create a custom static resource as follows:

```
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
_object = M2MInterfaceFactory::create_object("Test");
if(_object) {
    M2MObjectInstance* inst = _object->create_object_instance();
    if(inst) {
    	inst->create_static_resource("S",
        			     "ResourceTest",
                                     STATIC_VALUE,
                                     sizeof(STATIC_VALUE)-1);
```

In a Dynamic resource, the value is expected to change and everytime the server requests a GET operation, the value is fetched from setter APIs.

For example, you can create a custom dynamic resource as follows:

```
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
_object = M2MInterfaceFactory::create_object("Test");
if(_object) {
    M2MObjectInstance* inst = _object->create_object_instance();
    if(inst) {
	M2MResource* res = inst->create_dynamic_resource("D","ResourceTest",true);
        char buffer[20];
        int size = sprintf(buffer,"%d",_value);
        res->set_operation(M2MBase::GET_PUT_ALLOWED);
        res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
```
For more information on different functionalities of resources, please check the API documentation for M2MObject, M2MObjectInstance and M2MResource classes.

### Write

The **Write** operation is used to change the value of a Resource, an array of Resources Instances or multiple Resources from an Object Instance. The operation permits multiple Resources to be modified within the same instance of the operation. 

Whenever there is a valid PUT operation for any of the resources, the application will receive callback as follows:

```
void value_updated(M2MBase *base, M2MBase::BaseType type) 
```

Where `M2MBase` is the object whose value has been updated and `M2MBase::BaseType` tells what type of object has received updated values.


### Write Attributes

The **Write** operation is used to change the value of a Resource, an array of Resources Instances or multiple Resources from an Object Instance. The operation permits multiple Resources to be modified within the same instance of the operation. 

Any readable Resource can have attributes which are considered during the **Observe** operation. The following attributes are used:

- Minimum Period
- Maximum Period
- Greater Than
- Less Than
- Step

The **Write Attributes** operation is used to change the attributes of a Resource, an Object Instance, or an Object. The separation between **Write** and **Write Attributes** operations enables cache mechanism for the **Observe** operation. The operation permits multiple attributes to be modified within the same operation. The operation also can be used for cancelling the **Observe** operation.

The mbed Device server sets the endpoint attribute values that are used to determine when the endpoint sends the Resource value to the server.

Check the LWM2M specification for details on all the possible **Write Attributes** defined for different types of Objects and Resources.

### Execute

The **Execute** operation is used by the mbed Device Server to initiate some action, and can only be performed on individual Resources. The client MUST return an error when the **Execute** operation is received for Object Instances or Resource Instance.

To handle the **Execute** operation, the application can implement it as follows:

```
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"

    void M2MLWClient::execute_function(void */*argument*/) {
    }

_object = M2MInterfaceFactory::create_object("Test");
if(_object) {
    M2MObjectInstance* inst = _object->create_object_instance();
    if(inst) {
	M2MResource* res = inst->create_dynamic_resource("D","ResourceTest",true);
        char buffer[20];
        int size = sprintf(buffer,"%d",_value);
        res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
        res->set_value((const uint8_t*)buffer,
                       (const uint32_t)size);
        res->set_execute_function(execute_callback(this,&M2MLWClient::execute_function));
```

When the client receives the POST request for **Execute** from the mbed Device server for this resource, this function will be called and executed.

### Information Reporting Interface

The Information Reporting Interface is used by the mbed Device Server to observe any changes in a registered Resource on the client, receiving notifications when new values are available. This observation relationship is initiated by sending an **Observe** operation to the L2M2M Client for an Object, an Object Instance or a Resource. An observation ends when a **Cancel Observation** operation is performed. 

Information Reporting interface supports the following sub-features:

- Observe
- Notify
- Cancel

Currently, Information Reporting is handled only at Resources level. Support for Object and Object Instances will be added in later releases. 

### Observe

The mbed Device Server initiates an observation request for changes of Dynamic Resource, Resources within an Object Instance or for all the Object Instances of an Object within the client.

Related parameters for the **Observe** operation are described in [Write Attributes](#write-attributes) section.
When the **Observe** operation contains only Object ID, the **Notify** operation MUST be done per Object Instance.

To make your Resource observable, you need to set the Observable parameter as true for your object:

```
 object->set_observable(true);
```

If you want a Dynamic resource to be observable do the following when creating the resource:

```
M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         bool observable,
                                         bool multiple_instance =false);
```

The mbed Client will handle the observation part once you have defined the Resources to be observable.

### Notify

The **Notify** operation is sent from the client to the mbed Device Server during a valid observation on an Object Instance or a Resource. This operation includes the new value of the Object Instance or Resource. The **Notify** operation SHOULD be sent when all the conditions (Minimum Period, Maximum Period, Greater Than, Less Than, Step) configured by **Write Attributes** operation for **Observe** operation are met.

The mbed Client will send notifications for the Resources once the conditions are met.

### Cancel

The **Cancel Observation** operation is sent from the mbed Device Server to the client to end an observation relationship for an Object Instance or a Resource. The operation does not contain any parameters at the LWM2M layer. The **Cancel Observation** operation MUST be used in the response of the **Notify** operation.

The client handles both cancellation types:

 1. By sending **Cancel Observation** operation from the mbed Device server
 2. By sending **Write Attributes** with cancel parameter.

## More Information

This API is based on OMA LWM2M specification. You can get the specification [here](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0)

## How to use the API
More information on how to use the API effectively to create and configure Objects, Object Instances and Resources, can be found from [here](https://github.com/ARMmbed/mbed-client/blob/master/Howto.md)

## API documentation

You can generate Doxygen API documentation for these APIs from a doxy file which is present in the `doxygen` folder. You need to run `doxygen` command from the `doxygen/` folder and it will generate a `docs` folder at API source directory root level where you can find the detailed documentation for each API.

## Example application

An example application is available [here](https://github.com/ARMmbed/mbed-client-examples).

