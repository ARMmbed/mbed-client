# LWM2M-Client

## Introduction
This API enables the use of LWM2M features from the mbed Device Client. These features are described in the Lightweight Machine to Machine Technical Specification. Using this API, the mbed OS developers can develop their own applications utilizing the LWM2M features. 

This API provides an enabler that defines the application layer communication protocol between a LWM2M Server and a LWM2M Client located in a LWM2M Device. 

The OMA Lightweight M2M enabler includes device management and service enablement for LWM2M Devices. The target LWM2M Devices for this enabler are mainly resource constrained devices. Therefore, this enabler makes use of a light and compact protocol as well as an efficient resource data model.

A Client-Server architecture is introduced for the LWM2M Enabler. The LWM2M Device acts as the LWNM2M Client and the M2M service, platform or application acts as the LWM2M Server.

There are four interfaces between these two components as follows:

- Bootstrap
- Client Registration
- Device management and service enablement
- Information Reporting


##LWM2M Features

The API provides the application developer with an interface to define the application endpoint information that will be delivered to the Bootstrap Server and mbed Device Server during the bootstrap and registration operations respectively.

This is how you can create your interface for your endpoint:
```
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2minterface.h"

M2MInterface* interface = M2MInterfaceFactory::create_interface(*this,
                                                  "lwm2m-endpoint",
                                                  "test",
                                                  3600,
                                                  8000,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
```
Once you have created the interface, you can proceed to execute operations.

### Bootstrap feature

The Bootstrap Interface is used to provision essential information into the LWM2M Client to enable the LWM2M Client to perform the **Register** operation with one or more LWM2M Servers. 

In this release, only the client initiated bootstrap mode is supported.

### Client Initiated Bootstrap

The Client Initiated Bootstrap mode provides a mechanism for the LWM2M Client to retrieve the bootstrap information from a LWM2M Bootstrap Server. This mode requires a LWM2M Boostrap Server Account.

The User can provide the bootstrap server information and issue bootstrap command in following way.

First you need to create your bootstrap server object which contains information about bootstrap server like server address, security mode used by server etc.
```
#include "lwm2m-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, BOOTSTRAP_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 1);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
```

* Please note, currently this API supports only non-secure mode operations. Security features will be added in upcoming  releases.
 
Once you have the bootstrap object ready, all you need to do is to call bootstrap API by passing this object as parameter.
```
M2MInterface::bootstrap(M2MSecurity* bootstrap_object);
```

Since, this is an asynchronous operation, you will receive the result of this operation through callback defined in `m2minterfaceobserver.h` which you should be handling in your application.
If the bootstrap operation is successful and client is able to fecth the mbed Device Server information from bootstrap server, your application will receive following callback 
```
void bootstrap_done(M2MSecurity *server_object)
```

The `server_object` contains the data for mbed Device Server including serevr Uri, security mode etc. Using this object you can execute registration operation for this client.

In case, the  bootstrap operation fails for some reason , then you will receive following callback
```
void error(M2MInterface::Error error)
```

You can get to know more about the  error from the `error` parameter which is passed with the callback and then act accordingly.

###Client Registration Interface

The Client Registration Interface is used by a LWM2M Client to register with  LWM2M Servers, maintain  registration and de-register from a LWM2M Server. 
Currently, only one-to-one client server regisration is supported. But, in upcoming releases client API will support one-to many client-server registrations.

Client registration interface includes multiple sub-features and currently supported are:

- Register

- Update 

- De-register


###Register

When registering, the LWM2M Client performs the “Register” operation and provides the properties the LWM2M Server requires to contact the LWM2M Client (e.g., End Point Name); maintain the registration and session (e.g., Lifetime, Queue Mode) between the LWM2M Client and LWM2M Server as well as knowledge of the Objects the LWM2M Client supports and existing Object Instances in the LWM2M Client

This API enables clientregistration functionality.
User can provide the mbed Device server server information and issue register command in following way.

First you need to create your mbed Device  server object which contains information about bootstrap server like server address, security mode used by server etc.
If you have done bootstrap operation first, then this object is automatically created and available through `bootstrap_done()` callback which you can use directly.

```
#include "lwm2m-client/m2msecurity.h"
 M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::LWM2M);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, LWM2M_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::BootstrapServer, 0);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
```

* Please note, currently this API supports only non-secure mode operations. Security features will be added in upcoming  releases.

During registering your endpoint, you will also need to register all your resources that you would like to monitor or follow from mbed Deivce Server.
This can be easily achieved by creating resource objects and passing them to register API for registration purposes.

For example, if you want to register your OMA LWM2M Device object , you need to simply create the device object and set the values for manadatory resources like this.
```
#include "lwm2m-client/m2mdevice.h"
M2MDevice *device = M2MInterfaceFactory::create_device();
if(device) {
    device->create_resource(M2MDevice::Manufacturer,MANUFACTURER);
	device->create_resource(M2MDevice::DeviceType,TYPE);
	device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER);
	device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER);
    }
```
You can register other resources as well, please check detailed API documenttion for that.

Apart from manadatory device obejct, if you would like to register your own customized resources , you can create them and set their values accordingly.
For  doing that, please check the API documentation for M2MObject, M2MObjectInstance and M2MResource classes.

Once you have the registration server object and resources, which you want to register, ready, all you need to do is to call register API by passing these object as parameters.
```
M2MInterface::register_object(M2MSecurity* register_object, M2MObjectList object_list);
```

Since, this is an asynchronous operation, you will receive the result of this operation through callback defined in `m2minterfaceobserver.h` which you should be handling in your application.
If the register operation is successful and client is able to register all your resources to the mbed Device Server information , your application will receive following callback 
```
void object_registered(M2MSecurity *server_object, const M2MServer& server)
```

The `M2MSecurity *server_object` informs to which mbed Device Server the client has just registered and `M2MServer &server` contains the data related with mbed Device Server including Short ServerID, client registration period etc.

In case, the registration  operation fails for some reason , then you will receive following callback
```
void error(M2MInterface::Error error)
```

You can get to know more about the  error from the `error` parameter which is passed with the callback and then act accordingly.

###Update

Periodically or based on certain events within the LWM2M Client or initiated by the LWM2M Server, the LWM2M Client updates its registration information with a LWM2M Server by sending an “Update” operation to the LWM2M Server.
This is how you can de-update your registration.
```
M2MInterface::update_registration(M2MSecurity* security_object, const uint32_t lifetime)
```

Normally, the enabler will take care of updating the registration automatically but if you want to renew the regisrtaion before that , you can use this API.

If the update operation is successful, your application will receive following callback 
```
void registration_updated(M2MSecurity *const M2MServer& server)
```

The `M2MSecurity *server_object` informs to which mbed Device Server the client has just updated the registration and `M2MServer &server` contains the data related with mbed Device Server including Short ServerID, client registration period etc.

In case, the updation  operation fails for some reason , then you will receive following callback
```
void error(M2MInterface::Error error)
```

###De-register

When a LWM2M Client determines that it no longer requires to be available to a LWM2M Server (e.g., LWM2M Device factory reset), the LWM2M Client de-register from the LWM2M Server. Upon receiving this message, the LWM2M Server removes the registration information from the LWM2M Server.
This is how you can de-register your endpoint client. 
In case, endpoint has multiple server regsitration  then you need to provide the server_object of the server where you would like to de-register your endpoint. Otherwise if there is only one registration then you can pass NULL and client will unregister the default registration from endpoint.

```
M2MInterface::unregister_object(M2MSecurity *object);
```

Since, this is an asynchronous operation, you will receive the result of this operation through callback defined in `m2minterfaceobserver.h` which you should be handling in your application.

If the de-register operation is successful and client is successfully unregistered from mbed Device Server information , your application will receive following callback 
```
void object_unregistered(M2MSecurity *server_object)
```

The `M2MSecurity *server_object` informs to which mbed Device Server the client has just de-registered.

In case, the de-registration  operation fails for some reason , then you will receive following callback
```
void error(M2MInterface::Error error)
```

You can get to know more about the  error from the `error` parameter which is passed with the callback and then act accordingly.

###Device Management and Service Enablement Interface

The Device Management and Service Enable Interface is used by the LWM2M Server to access Object Instances and Resources available from the LWM2M Client. The interface provides this access through the use of “Create”, “Read”, “Write”, “Delete”, “Execute”, “Write Attributes”, or “Discover” operations.

Device Management and Service Enable Interface includes sub-features and currently supported are:

 - Read
 
 - Write
 
 - Write Attributes

 - Execute

Currently, Device Management and Service Enable Interface is handled only at Resources level, support for Object and Object Instances will be added in later releases.

###Read

The “Read” operation is used to access the value of a Resource, an array of Resource Instances, an Object Instance or all the Object Instances of an Object.
Client API enables to set the value of resources which can be read by LWM2M server.

There are two types of resources that you can create

 - Static

 - Dynamic

In Static resource, you set the value of the resource once and it doesn't change during the course of operations. Static resources can be used to report values like Device name, type, address etc.

For example, You can create custom static resource like this

```
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"
_object = M2MInterfaceFactory::create_object("Test");
if(_object) {
    M2MObjectInstance* inst = _object->create_object_instance();
    if(inst) {
    	inst->create_static_resource("S",
        			     "ResourceTest",
                                     STATIC_VALUE,
                                     sizeof(STATIC_VALUE)-1);
```

In Dynamic resource, it is expected for the value to change and everytime the server requests GET operation, value is fetched from setter APIs.

For example, You can create custom dynamic resource like this

```
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"
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
For more information on different functionality of resources, please check the API documentation for M2MObject, M2MObjectInstance and M2MResource classes.

###Write

The “Write” operation is used to change the value of a Resource, an array of Resources Instances or multiple Resources from an Object Instance. The operation permits multiple Resources to be modified within the same instance of the operation. 

Whenever there is a valid PUT operation for any of the resource then the application will receive the callback

```
void value_updated(M2MBase *base, M2MBase::BaseType type) 
```

where M2MBase is the object whose value has been updated and M2MBase::BaseType tells what type of object has received updated values.


###Write Attribute

The “Write” operation is used to change the value of a Resource, an array of Resources Instances or multiple Resources from an Object Instance. The operation permits multiple Resources to be modified within the same instance of the operation. 

Any readable Resource can have attributes which are considered during the “Observe” operation. The following attributes are used and further explained in the table below: Minimum Period, Maximum Period, Greater Than, Less Than, and Step.
The “Write Attributes” operation is used to change the attributes of a Resource, an Object Instance, or an Object. The separation between “Write” and “Write Attributes” operations enables cache mechanism for the Observe operation. The operation permits multiple attributes to be modified within the same operation. The operation also can be used for cancelling the “Observe” operation.

The value set by the Resources are sent to the LWM2M server based on the criteria set by these attributes set from LWM2M server.
Check the LWM2M specification for details on all the possible Write Attributes defined for different types of Objects and Resources.

###Execute
The “Execute” operation is used by the LWM2M Server to initiate some action, and can only be performed on individual Resources. A LWM2M Client MUST return an error when the “Execute” operation is received for anObject Instance(s) or Resource Instance(s).

In order to handle the Execute operation, the application can implement it in following way

```
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"

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
When the client will receive POST request for Execute from LWM2M server for this resource, then this function will be called and executed.

###Information Reporting Interface

The Information Reporting Interface is used by a LWM2M Server to observe any changes in registered Resource on a LWM2M Client, receiving notifications when new values are available. This observation relationship is initiated by sending an “Observe” operation to the L2M2M Client for an Object, an Object Instance or a Resource. An observation ends when a “Cancel Observation” operation is performed. 

Information Reporting interface includes sub-features and currently supported are:

 - Observe

 - Notify

 - Cancel

Currently, Information Reporting is handled only at Resources level, support for Object and Object Instances will be added in later releases. 

###Observe
The LWM2M Server initiates an observation request for changes of Dynamic Resource, Resources within an Object Instance or for all the Object Instances of an Object within the LWM2M Client.
Related parameters for “Observe” operation are described in *Write Attributes* section.
When “Observe” operation contains only Object ID, the “Notify” operation MUST be done per Object Instance.

In order to make your Resource observable , you need to set the Observable parameter as true for your object.

```
 object->set_observable(true);
```

For Dynamic resource, while creating the resource, you can define if you want the resource to be observable

```
M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         bool observable,
                                         bool multiple_instance =false);
```

LWM2M mbed Client will handle the observation part once the Resources have defined if they want to be observable.

###Notify

The “Notify” operation is sent from the LWM2M Client to the LWM2M Server during a valid observation on an Object Instance or Resource. This operation includes the new value of the Object Instance or Resource. The “Notify” operation SHOULD be sent when all the conditions (i.e., Minimum Period, Maximum Period, Greater Than, Less Than, Step) configured by “Write Attributes” operation for “Observe” operation are met.

LWM2M mbed Client will send notifications for the Resources once the conditions are met.

###Cancel

The “Cancel Observation” operation is sent from the LWM2M Server to the LWM2M Client to end an observation relationship for Object Instance or Resource. The operation doesn’t contain any parameters at the LWM2M layer. The “Cancel Observation” operation MUST be used in the response of the “Notify” operation.

LWM2M Client handles both the types of cancellation 
 1.  By sending “Cancel Observation” operation from LWM2M server
 2.  2.	By sending “Write Attributes” with cancel parameter.

##More Information

This API is based on OMA LWM2M specification. You can get specification from [here]()

##API documentation

You can generate Doxygen API documentation for these APIs from doxy file which is present in `cd doxygen` folder . You need to run `doxygen` command from the `doxygen/` folder and it will generate `docs` folder at API source directory root level where you can find the detailed documentation for each API.

##Example application
In order to use these API, there is an example application available [here](https://github.com/ARMmbed/lwm2m-client-example)

