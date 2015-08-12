# mbed Client

## Introduction

mbed Client is an OS-agnostic embedded software library that provides the means to connect and manage constrained embedded devices to web applications through the mbed Device Server. 

The mbed Device Client API allows mbed OS developers to create applications with LWM2M features. These features are described in the [Lightweight Machine to Machine Technical Specification](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0); they include high level APIs to manage devices on the mbed Device Server, securely communicate with internet services over the industry standard TLS/DTLS, and fully control the endpoint and application logic. 

The API is written in C++ to allow quick application development.

There are three interfaces between the mbed Device Server (mDS) and mbed Client:

- Client registration and deregistration.
- Device management and service enabler.
- Information reporting.

##mbed Client interfaces
- Client registration and deregistration
- Device management and service enablement
- Information reporting

The API provides an interface to define the application endpoint information. This information will be delivered to mbed Device Server during the registration operation (explained below).

To create an interface for your endpoint:

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

### Client Registration Interface

The client uses the Client Registration Interface to register with mDS, update registration and deregister.

Currently, only one-to-one client-server registration is supported. One-to-many client-server registrations will be supported in an upcoming release.

The Client Registration Interface includes multiple sub-features. Currently supported:

- Register
- Update 
- Deregister

### The Register feature

This API enables the client registration functionality.

When registering, the client:

* Performs the **Register** operation and provides parameters that mDS requires to register the client (for example End Point Name).

* Maintains the registration and session (for example Lifetime, Queue Mode).

* Provides information on the Objects the client supports and existing Object Instances in the client.

#### Registering your client

To provide information to the mbed Device Server and issue the register command:

First you need to create an mbed Device Server object. This object contains information about the mbed Device Server, such as its address and security mode.

```
#include "mbed-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::LWM2M);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, LWM2M_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 0);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
```

**Note**: This API supports both non-secure and secure mode operations. For secure mode , you will also need to provide certificate, private key and server public key through the API.

Next, you need to register all the resources that you would like to monitor or follow via the mbed Device Server. To do this, create the resource objects and pass them to the Register API for registration purposes.

For example, if you want to register your OMA LWM2M based Device object, you need to create the object and set the values for mandatory resources as follows:

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

Note: You can register other resources, including custom resources. Please check the API documentation for a detailed description of the M2MObject, M2MObjectInstance and M2MResource classes.

You have the registration server object and resources that you want to register. Now you need to call the register API and pass the following objects as parameters:

```
M2MInterface::register_object(M2MSecurity* register_object, M2MObjectList object_list);
```

___Success or failure callback___

Because this is an asynchronous operation, you will receive the result of this operation through a callback defined in `m2minterfaceobserver.h` in your application.

If the register operation is successful and the client can register all your resources to mDS, your application will receive the following callback:

```
void object_registered(M2MSecurity *server_object, const M2MServer& server)
```

The `M2MSecurity *server_object` specifies to which mbed Device Server the client has just registered and `M2MServer &server` contains the data related to mDS, including the Short ServerID and the client registration period.

If the registration operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback; use it to fix the source of the error.

### Update

Periodically, or in response to events within the client or as initiated by the mbed Device Server, the client updates its registration information with mDS. It does this by sending an **Update** operation to mDS.

To update your registration:

```
M2MInterface::update_registration(M2MSecurity* security_object, const uint32_t lifetime)
```

Normally, the enabler will update the registration automatically, but if you want to renew the registration before that, you can use this API.

___Success or failure callback___

If the update operation is successful, your application will receive the following callback:

```
void registration_updated(M2MSecurity *const M2MServer& server)
```

The `M2MSecurity *server_object` specifies to which mbed Device Server the client has just updated the registration and `M2MServer &server` contains the data related to mDS, including the Short ServerID and the client registration period.

If the update operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

### Deregister

The client can deregister from mDS when it no longer requires access to the server. When mDS receives the Deregister message it removes the device's registration information from its database. When the client next needs mDS, it will have to register again.

To deregister your endpoint client:

If the endpoint has multiple server registrations, you need to provide the `server_object` of the server where you would like to deregister your endpoint. Otherwise, if there is only one registration, you can pass `NULL` and the client will deregister the default registration.

```
M2MInterface::unregister_object(M2MSecurity *object);
```

___Success or failure callback___

Because this is an asynchronous operation, you will receive the result of this operation through a callback defined in `m2minterfaceobserver.h` in your application.

If the client is successfully deregistered from mDS, your application will receive the following callback:

```
void object_unregistered(M2MSecurity *server_object)
```

The `M2MSecurity *server_object` specifies from which mbed Device Server the client has just deregistered.

If the deregistration operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback; use it to fix the source of the problem.

### Device Management and Service Enabler Interface

The mbed Device Server uses the Device Management and Service Enabler Interface to access Object Instances and Resources available on the client. The interface provides this access through the following operations:

- **Create**
- **Delete**
- **Read**
- **Write**
- **Execute**
- **Write Attributes**

Currently, support for the Create and Delete actions is limited to Object Instances.

The Device Management and Service Enabler Interface supports the following data types:

* Text - for Resources.

* TLV - for Object and Object Instances.

### Read

The Client API allows setting values to Resources, an array of Resource Instances, an Object Instance or all the Object Instances of an Object (TLV format supported). mDS can then read these values using the **Read** operation.

___Creating Resources___

There are two types of resources you can create:

- Static: you set the value of the resource once and it does not change during the course of operations.

- Dynamic: the value is expected to change during the course of operations. Therefore, the value is fetched from setter APIs every time the server requests a GET operation.

Here is an example of creating a custom static Resource:

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

And here is an example of creating a custom dynamic Resource:

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

For more information on different resource functionalities, please check the API documentation for the M2MObject, M2MObjectInstance and M2MResource classes.

### Write

The **Write** operation is used to overwrite the value of a Resource, an array of Resource Instances or multiple Resources from an Object Instance. 

Whenever there is a valid PUT operation for any of the resources, the application will receive a callback:

```
void value_updated(M2MBase *base, M2MBase::BaseType type) 
```

Where `M2MBase` is the object whose value has been updated and `M2MBase::BaseType` is the object's type.

### Write Attributes

Any readable Resource can have attributes that are considered during the **Observe** operation (explained below). The following attributes are used:

- Minimum Period (pmin)
- Maximum Period (pmax)
- Greater Than (gt)
- Less Than (lt)
- Step (st)

mDS sets the endpoint attribute values that are used to determine when the endpoint sends the Resource value to the server.

Check the LWM2M specification for details of all the possible **Write Attributes** defined for different types of Objects and Resources.

### Execute

mDS uses the **Execute** operation to perform an action. This operation can only be performed on individual Resources. 

Note: the client **must** return an error when the **Execute** operation is received for Object Instances or Resource Instances.

Here is an implementation example for the **Execute** operation:

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

When the client receives the POST request for Execute from mDS for this resource, this function will be called and executed.

### Information Reporting Interface

mDS uses the Information Reporting Interface to observe any changes in a registered Resource on the client, receiving notifications when new values are available. 

The interface supports the following sub-features:

- Observe
- Notify
- Cancel

### Observe

mDS initiates an observation request to change the value of a dynamic Resource.

Tip: related parameters for the **Observe** operation are described in the [Write Attributes](#write-attributes) section.

To make your Resource observable, you need to set the Observable parameter of your object to TRUE:

```
 object->set_observable(true);
```

If you want a dynamic resource to be observable, do the following when creating the resource:

```
M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         bool observable,
                                         bool multiple_instance =false);
```

The mbed Client will handle the observation part once you have defined the Resources to be observable.

### Notify

The client sends the **Notify** operation to mDS during a valid observation on a Resource, once the notification conditions are met.

### Cancel

mDS sends the **Cancel Observation** operation to the client to end an observation relationship for an Object Instance or a Resource.

## More Information

This API is based on OMA LWM2M specification. You can get the specification [here](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0).

## How to use the API
More information on how to use the API effectively to create and configure Objects, Object Instances and Resources, can be found [here](https://github.com/ARMmbed/mbed-client/blob/master/Howto.md)

## API documentation

You can generate Doxygen API documentation for these APIs from a doxy file present in the `doxygen` folder. You need to run the `doxygen` command from the `doxygen/` folder; it will generate a `docs` folder at the API source directory root level. The folder contains detailed documentation for each API.

## Example application

An example application is available [here](https://github.com/ARMmbed/mbed-client-examples).

