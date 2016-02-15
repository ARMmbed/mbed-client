# The Device Management and Service Enabler feature

mbed Device Server uses the Device Management and Service Enabler to access Object Instances and Resources available on the client. The interface provides this access through the following operations:

- [Create](#the-create-operation)
- Delete
- [Read](#the-read-operation)
- [Write](#the-write-operation)
- [Write Attributes](#the-write-attributes-operation)
- [Execute](#the-execute-operation)

Currently, support for the Create and Delete actions is limited to Object Instances.

The Device Management and Service Enabler Interface supports the following data types:

- Text: for Resources.
- TLV: for Object and Object Instances.

## The Create operation

There are two types of resources you can create:

- Static: you set the value of the resource once and it does not change during the course of operations.
- Dynamic: the value is expected to change during the course of operations. Therefore, the value is fetched from setter APIs every time the server requests a read operation.

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

## The Read operation

The Client API allows setting values to Resources, an array of Resource Instances, an Object Instance or all the Object Instances of an Object (TLV format supported). mbed Device Server can then read these values using the **Read** operation.

## The Write operation

The **Write** operation is used to overwrite the value of a Resource, an array of Resource Instances or multiple Resources from an Object Instance. 

Whenever there is a valid `PUT` operation for any of the resources, the application will receive a callback:

```
void value_updated(M2MBase *base, M2MBase::BaseType type) 
```

Where `M2MBase` is the object whose value has been updated and `M2MBase::BaseType` is the object type.

## The Write Attributes operation

Any readable Resource can have attributes that are considered during the [**Observe** operation](info_reporting.md#the-observe-feature). The following attributes are used:

- Minimum Period (pmin)
- Maximum Period (pmax)
- Greater Than (gt)
- Less Than (lt)
- Step (st)

mbed Device Server sets the endpoint attribute values that are used to determine when the endpoint sends the Resource value to the server.

Check the [LWM2M Specification](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0) for details of all the possible **Write Attributes** defined for different types of Objects and Resources.

## The Execute operation

mbed Device Server uses the **Execute** operation to perform an action. This operation can only be performed on individual Resources. 

**Note:** mbed Client returns an error when the **Execute** operation is received for Object Instances or Resource Instances.

Here is an implementation example for the **Execute** operation. If you want to execute a piece of code in your application, you can do it by passing it the `POST` request from mbed Device Server:

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

