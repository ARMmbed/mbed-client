# ARM mbed Client overview

ARM mbed Client is a library that provides the means to connect constrained embedded devices to mbed Device Connector Service, mbed Device Server and to mbed-enabled cloud services from our partners.

The mbed Client high-level APIs allow mbed OS developers to create applications with LWM2M features as described in the [Lightweight Machine to Machine Technical Specification](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0):

- Manage devices on mbed Device Server.
- Securely communicate with internet services over the industry standard TLS/DTLS.
- Fully control the endpoint and application logic. 

The API is written in C++ to allow quick application development.

## Manage devices on mbed Device Server

There are three interfaces between mbed Device Server and mbed Client:

- Client Registration and Deregistration
- Device Management and Service Enablement
- Information Reporting

The API also provides an interface to define the application endpoint information. This information will be delivered to mbed Device Server during the registration operation.

To create an interface for your endpoint (client):

```
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterface.h"

M2MInterface* interface = M2MInterfaceFactory::create_interface(*this,
                                                  "mbed-endpoint",
                                                  "test",
                                                  3600,
                                                  5684,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
```


### The Client Registration Interface

The client uses the Client Registration Interface to register with mbed Device Server, update registration and deregister.

Currently, only one-to-one client-server registration is supported. One-to-many client-server registrations will be supported in an upcoming release.

The Client Registration Interface includes multiple sub-features. Currently supported:

- Register
- Update 
- Deregister

### The Register feature

This API enables the client registration functionality.

When registering, the client:

* Performs the **Register** operation and provides parameters that mbed Device Server requires to register the client (for example Endpoint Name).

* Maintains the registration and session (for example Lifetime, Queue Mode).

* Provides information on the Objects the client supports and existing Object Instances in the client.

##### Registering your client

To provide information to mbed Device Server and issue the register command:

**Step 1.** Create an mbed DS Object. This object contains information about mbed Device Server, such as its address and security mode.

```
#include "mbed-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, LWM2M_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 0);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
```

**Step 2.** mbed Client supports both non-secure and secure mode operations. For secure mode, you need to provide a certificate, private key and server public key through the API.

To create a secure mode operation:

```
#include "mbed-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, LWM2M_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 0);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
            security->set_resource_value(M2MSecurity::ServerPublicKey,<SERVER_CERT>,sizeof(<SERVER_CERT>));
            security->set_resource_value(M2MSecurity::PublicKey,<CERT>,sizeof(<CERT>));
            security->set_resource_value(M2MSecurity::Secretkey,<KEY>,sizeof(<KEY>));   
        }
```

**Step 3.** Register all the resources that you would like to monitor or follow using mbed Device Server. To do this, create the resource objects and pass them to the Register API for registration purposes.

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

**Note:** You can register other resources, including custom resources. Please check the [API documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/annotated.html) for a detailed description of the M2MObject, M2MObjectInstance and M2MResource classes.

**Step 4.** You have the registration server object and resources that you want to register. Now, you need to call the register API and pass the following objects as parameters:

```
M2MInterface::register_object(M2MSecurity* register_object, M2MObjectList object_list);
```

**Success or failure callback**

Because this is an asynchronous operation, you will receive the result of this operation through a callback defined in `m2minterfaceobserver.h` in your application.

If the register operation is successful and the client can register all your resources to mbed DS, your application will receive the following callback:

```
void object_registered(M2MSecurity *server_object, const M2MServer& server)
```

The `M2MSecurity *server_object` specifies to which mbed DS instance the client has just registered and `M2MServer &server` contains the data related to mbed DS, including the Short ServerID and the client registration period.

If the registration operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback; use it to fix the source of the error.

##### The Update feature

Periodically, or in response to events within the client or as initiated by mbed Device Server, the client updates its registration information with mbed Device Server by sending it an **Update** operation.

Normally, the enabler will update the registration automatically, but if you want to renew the registration before that, you can use this API.

To update your registration:

```
M2MInterface::update_registration(M2MSecurity* security_object, const uint32_t lifetime)
```

**Success or failure callback**

_Success_

If the update operation is successful, your application will receive the following callback:

```
void registration_updated(M2MSecurity *const M2MServer& server)
```

The `M2MSecurity *server_object` specifies with which mbed Device Server instance the client has just updated the registration and `M2MServer &server` contains the data related to mbed Device Server, including the Short ServerID and the client registration period.

_Failure_

If the update operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

#### The Deregister feature

The client can deregister from mbed Device Server when it no longer requires access to the server. When mbed Device Server receives the **Deregister** message it removes the device's registration information from its database. When the client needs mbed Device Server again, it has to register again.

To deregister your endpoint client:

If the endpoint has multiple server registrations, you need to provide the `server_object` of the server from which would like to deregister. If there is only one registration, you can pass `NULL` and the client will deregister from the default registration.

```
M2MInterface::unregister_object(M2MSecurity *object);
```

**Success or failure callback**

Because this is an asynchronous operation, you will receive the result of this operation through a callback defined in `m2minterfaceobserver.h` in your application.

_Success_

If the client is successfully deregistered from mbed Device Server, your application will receive the following callback:

```
void object_unregistered(M2MSecurity *server_object)
```

The `M2MSecurity *server_object` specifies from which mbed Device Server instance the client has just deregistered.

_Failure_

If the deregistration operation fails for some reason, you will receive the following callback:

```
void error(M2MInterface::Error error)
```

You will get more information about the error from the `error` parameter passed with the callback; use it to fix the source of the problem.

### The Device Management and Service Enabler Interface

mbed Device Server uses the Device Management and Service Enabler Interface to access Object Instances and Resources available on the client. The interface provides this access through the following operations:

- Create
- Delete
- Read
- Write
- Execute
- Write Attributes

Currently, support for the Create and Delete actions is limited to Object Instances.

The Device Management and Service Enabler Interface supports the following data types:

- Text: for Resources.
- TLV: for Object and Object Instances.



#### The Create operation

There are two types of resources you can create:

- Static: you set the value of the resource once and it does not change during the course of operations.
- Dynamic: the value is expected to change during the course of operations. Therefore, the value is fetched from setter APIs every time the server requests a `GET` operation.

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

For more information on different resource functionalities, please check the [API documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/annotated.html) for the M2MObject, M2MObjectInstance and M2MResource classes.

#### The Read operation

The Client API allows setting values to Resources, an array of Resource Instances, an Object Instance or all the Object Instances of an Object (TLV format supported). mbed Device Server can then read these values using the **Read** operation.

#### The Write operation

The **Write** operation is used to overwrite the value of a Resource, an array of Resource Instances or multiple Resources from an Object Instance. 

Whenever there is a valid `PUT` operation for any of the resources, the application will receive a callback:

```
void value_updated(M2MBase *base, M2MBase::BaseType type) 
```

Where `M2MBase` is the object whose value has been updated and `M2MBase::BaseType` is the object type.

#### The Write Attributes operation

Any readable Resource can have attributes that are considered during the [**Observe** operation](#the-observe-feature). The following attributes are used:

- Minimum Period (pmin)
- Maximum Period (pmax)
- Greater Than (gt)
- Less Than (lt)
- Step (st)

mbed Device Server sets the endpoint attribute values that are used to determine when the endpoint sends the Resource value to the server.

Check the [LWM2M Specification](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0) for details of all the possible **Write Attributes** defined for different types of Objects and Resources.

#### The Execute operation

mbed Device Server uses the **Execute** operation to perform an action. This operation can only be performed on individual Resources. 

**Note:** The client **must** return an error when the **Execute** operation is received for Object Instances or Resource Instances.

Here is an implementation example for the **Execute** operation. This function is called when the client receives the `POST` request for this resource from mbed Device Server:

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


### The Information Reporting Interface

mbed Device Server uses the Information Reporting Interface to observe any changes in a registered Resource on the client.The server registers to observe the Resource. When the Resource changes, it sends a notification with its new value to all servers who asked to observe it.

The interface supports the following sub-features:

- Observe
- Notify
- Cancel

#### The Observe feature

mbed Device Server initiates an observation request to see the value of a Dynamic Resource.

**Tip:** Related parameters for the **Observe** operation are described in the [Write Attributes](#write-attributes) section.

To make your Resource observable, you need to set the Observable parameter of your object to `true`:

```
 object->set_observable(true);
```

If you want a dynamic Resource to be observable, do the following when creating the resource:

```
M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         M2MResourceInstance::ResourceType type,
                                         bool observable,
                                         bool multiple_instance =false);
```

The mbed Client will handle the observation part once you have defined the Resources to be observable.

#### The Notify feature

The client sends the **Notify** operation to mbed Device Server during a valid observation on a Resource, when the notification conditions are met.

#### Cancel

mbed Device Server sends the **Cancel Observation** operation to the client to end an observation relationship for an Object Instance or a Resource.


## How to use the API
More information on how to use the API effectively to create and configure Objects, Object Instances and Resources, can be found [here](Howto.md).

## API documentation

The Doxygen for this API is [available here](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/annotated.html).

## Example application

We have an example application for

1. [mbed OS](https://github.com/ARMmbed/mbed-client-examples).

2. [Ubuntu](https://github.com/ARMmbed/mbed-client-linux-example).

