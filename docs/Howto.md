# How to use the mbed Client API

This section explains how to use our C++ API to create and configure different types of Objects, Object Instances and Resources for mbed Client to comply with the OMA LWM2M specifications. These resources can then be communicated to mbed Device Server.

**Note:** The API complies with the OMA LWM2M specifications.

As per the OMA LWM2M specification:

- The client must have defined Objects, under which it can create Object Instances.
- The client must have created Object Instances under Objects, which will eventually contain Resources.
- The client must have Resources under Object Instances which belong to Objects.
- The client must have created Resources under Object Instance. You can create M2MResource from the M2MObjectInstance class.
- The client must have created Resource Instances under Resources. You can create M2MResourceInstance from the M2MObjectInstance class.


## How to create and configure Objects

With this API, you can create and configure the following Objects:

- [Device Objects](#device-object)
- [Security Objects](#security-object)
- [Custom Objects](#custom-object)

The M2MObject class is derived from the M2MBase class, so all the public methods can be used from the M2MObject and its derived classes.

### Creating OMA-defined Objects

#### Device Object

To create a Device Object:

`static M2MDevice *create_device();`

Because there can be only one instance of M2MDevice, it is a static class and you can delete it as follows:

`M2MDevice::delete_instance();`

Check the [M2MDevice class documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/classM2MDevice.html) to see how to configure the Device Object, as well as how to create appropriate Resources and assign values to them. 

#### Security Object

To create a Security Object:

`static M2MSecurity *create_security(M2MSecurity::ServerType server_type);`

You can create a Bootstrap or normal mbed Device Server by passing the appropriate `enum` value.

Check the [M2MSecurity class documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/classM2MSecurity.html) to see how to configure the Security Object, as well as how to create appropriate Resources and assign values to them.

#### Custom Object

For a Custom Object, you need to pass the name of the Object that you would like to create (for example `Test`); this will create an Object with that name in mbed Device Server.

To create an M2MObject:

`static M2MObject *create_object(const String &name);`

#### Configuring the Object

Once you have created an Object (whether OMA-specific or custom), you can configure various parameters in that Object so that they can be controlled or modified to affect communication with mbed Device Server.

Here, we discuss a few of the most important parameters, which you must configure properly to work with the Objects.

##### Setting Operation Mode

To set the operation mode of the Objects so that they can handle `GET`, `PUT`, `POST`, `DELETE` or a combination of these requests coming from mbed Device Server:

`virtual void set_operation(M2MBase::Operation operation);`

##### Setting Observable Mode

To set the Object to be an observing resource:

`virtual void set_observable(bool observable);`

##### Setting CoAP content type

Currently, the only available content type is the OMA TLV type. The OMA TLV type works only for Objects with a numeric value. For example, if you are creating a custom Object it must be of a numeric type like `100`.

If you want your Object to support the TLV type, set the object's CoAP content type as `99`:

`virtual void set_coap_content_type(const uint8_t content_type);`

**Tip:** In future releases, we will introduce support for the JSON content types.

## How to create and configure Object Instances

With this API, you can create and configure the following Object Instances:

- [Device Object Instance](#device-object-instance)
- [Security Object Instance](#security-object-instance)
- [Custom Object Instance](#custom-object-instance)

The M2MObjectInstance class is derived from the M2MBase class, so all the public methods from M2MObjectInstance and its derived classes can be used.

### Creating an OMA-defined Object Instance

#### Device Object Instance

Because there can only be one instance for the Device Object, the ObjectInstance is automatically created when creating an M2MDevice Object.

#### Security Object Instance

Because there can only be one instance for the Security Object, the Object Instance is automatically created when creating an M2MDevice Object based on a selected server type.

#### Custom Object Instances

Object Instances need IDs. Normally, the IDs start from '0' and increment, 
so the Object Instance structure on the mbed Device Server side would look like this:

`Object/Object Instance ID`

When you create an Object Instance, you therefore need to pass the ID that you would like to create. For example, if you pass `0`, you get an object `/Test/0` on mbed Device Server.

To create an M2M Object Instance:

`M2MObject::create_object_instance(uint16_t instance_id);`

You can pass an Object Instance ID to create appropriate Object Instances. Normally, Object Instances will start from `0` and increment. So the Object Instance structure on the mbed DS side would look like this:

`Object/Object Instance ID`

### Configuring the Object Instance

When you have created an Object Instance (whether OMA-specific or custom), you can configure various parameters in that object so that they can be controlled or modified to affect communication with mbed Device Server.

Here, we present a few of the most important parameters that you must configure properly to work with the object instances.

#### Setting operation mode

You can set the operation mode of the Object Instances so that they can handle `GET`, `PUT`, `POST`, `DELETE` or a combination of these requests coming from mbed Device Server.

The API that you can use to set the operation mode (present in the M2MBase class) is:

`virtual void set_operation(M2MBase::Operation operation);`

#### Setting observable mode

To set the object to be an observing resource:

You can set the observable mode using the following API:

`virtual void set_observable(bool observable);`

#### Setting CoAP content type

Currently, the only available content type is the OMA TLV type. The OMA TLV type works only for Objects with a numeric value. For example, if are creating a custom object it must be of a numeric type like `100`.

If you want your Object to support the TLV type, set the CoAP content type of the object to `99`:

`virtual void set_coap_content_type(const uint8_t content_type);`

**Tip:** In future version, we will introduce support for the JSON content types.

There are additional APIs that provide getter and remove functions for Object Instances in the M2MObjectInstance class; [check the API documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/classM2MObjectInstance.html) for their usage. 

## How to create and configure Resources and Resource Instances

With this API, you can create and configure the following Resources and Resource Instances:

- [Device Object Resources](#device-object-resources)
- [Security Object Resources](#security-object-resources)
- [Custom Resources](#custom-resources)


The M2MResource class is derived from the M2MResourceInstance, which in turn is derived from the M2MBase class, so all the public methods can be used from M2MResource or M2MResourceInstance and their derived classes.

### Creating OMA-defined Resources

#### Device Object Resources

There are direct APIs to create and set values for the Device Resources. You can create the required Resource and set values based on their data types.

- For Resources that take string values:

  `M2MResource* create_resource(DeviceResource resource, const String &value);`

- For Resources that take integer values:

  `M2MResource* create_resource(DeviceResource resource, uint32_t value);`

- There are a few Resources that can have multiple instances. To create these Resources:

  `M2MResourceInstance* create_resource_instance(DeviceResource resource, uint32_t value,uint16_t instance_id);`

  Where `instance_id` is the Resource Instance ID, for example `/3/0/11/0`.

Check the [M2MDevice API documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/classM2MDevice.html) to find which enums are supported for `integer` or `string` value types.

There are other APIs in the M2MDevice class that you can use to set, remove and modify new values for the resources.

#### Security Object Resources

_Mandatory Resources_

Most of the mandatory Resources are created automatically when you create an M2MSecurity Object. You can set their values based on their data types:

- For Resources that take integer values:

  `bool set_resource_value(SecurityResource resource,uint32_t value);`

- For Resources that take string values:

  `bool set_resource_value(SecurityResource resource,const String &value);`

- For Resources that take binary values, like setting public keys and certificates:

  `bool set_resource_value(SecurityResource resource,onst uint8_t *value,const uint16_t length);`
  
_Optional Resources_

To create and set values for Resources that are not mandatory, and which take an integer value:

  `M2MResource* create_resource(SecurityResource resource, uint32_t value);`

Check the [M2MSecurity API documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/classM2MSecurity.html) to find which enums are supported for `integer`, `string` or `uint8_t*` value types.

There are more APIs in the M2MSecurity class that you can use to set, remove and modify Resource values.

#### Custom Resources

For Custom Objects, you can create Resources of two types:

- **M2MResource**: a resource with a single instance, for example `/Test/0/Resource`.

- **M2MResourceInstance**: a resource with multiple instances, for example `/Test/0/Resource/0`, `/Test/0/Resource/1`.

For each of these types, the Resource and Resource Instances can be either static or dynamic:

- Static: Resource and Resource Instances whose value does not change over time.
- Dynamic: Resource and Resource Instances whose value can change. These can be made observable.

**Creating dynamic and static single-instance Resources**

- To create a single-instance Resource with a static value:

```	
M2MResource* create_static_resource(const String &resource_name,
const String &resource_type, M2MResourceInstance::ResourceType type,
const uint8_t *value, const uint8_t value_length,
bool multiple_instance = false);
```

- To create a single-instance Resource with a dynamic value that can be set later on:

```
M2MResource* create_dynamic_resource(const String &resource_name,
const String &resource_type, M2MResourceInstance::ResourceType type,
bool observable, bool multiple_instance = false);
```

These APIs take different parameters that you can see in the documentation.

**Creating dynamic and static Resource Instances**

- To create a Resource Instance with a static value:

```
M2MResourceInstance* create_static_resource_instance(const String &resource_name,
const String &resource_type, M2MResourceInstance::ResourceType type,
const uint8_t *value, const uint8_t value_length, uint16_t instance_id);
```

- To create a Resource Instance with a dynamic value that can be set later on:

```
M2MResourceInstance* create_dynamic_resource_instance(const String &resource_name,
const String &resource_type, M2MResourceInstance::ResourceType type,
bool observable, uint16_t instance_id);
```

These APIs take different parameters that you can see in the documentation.

#### Configuring the Resource and Resource Instance

When you have created a Resource or Resource Instance (whether OMA-specific or custom), you can configure various parameters so they can be controlled or modified to affect communication with mbed Device Server.

Here, we present a few of the most important parameters that you must configure properly to work with the Resource and Resource Instance.

##### Setting Operation Mode

You can set the Resource or Resource Instance operation mode so that they can handle `GET`, `PUT`, `POST`, `DELETE` or a combination of these requests coming from mbed Device Server.

To set the operation mode:

  `virtual void set_operation(M2MBase::Operation operation);`

##### Setting the value of a dynamic Resource or Resource Instance

You can set the value of a dynamic Resource or Resource Instance so that they can be sent to mbed Device Server using `GET` requests.

To set the values :

  `virtual bool set_value(const uint8_t *value, const uint32_t value_length);`

##### Setting an executable function

For Dynamic Resources, you can pass a function pointer to the Resource or Resource Instance. It will be executed when mbed Device Server calls a `POST` method on that resource. The Resource or Resource Instance must support the `POST` operation mode for this feature to work.

To pass the function pointer:

`virtual void set_execute_function(execute_callback callback);`

There are additional APIs that provide getter and remove functions for Resource and Resource Instances in the M2MResource and M2MResourceInstance classes. Check [the API documentation](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/annotated.html) for their usage. 
