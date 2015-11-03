#How to use the mbed Client API

This section explains how to create different types of Objects, Object Instances and Resources for the client to comply with the OMA LWM2M 
specifications.

In order for the client to communicate its resources to the mbed Device Server (mbed DS), it needs to create Objects, Object Instances and Resources. This can be done very easily with the Client C++ APIs, where the client can define its resources in a similar structure to the one defined in the LWM2M specification.

This quick guide will explain how you can create and configure these resources using the C++ API.

##How to create and configure Objects

The M2MObject class is derived from the M2MBase class, so all the public methods can be used from M2MObject and its derived classes.

###Creating OMA-defined Objects

####Device Object

There is a direct API in the M2MInterfaceFactory class to create Device Object:

```static M2MDevice *create_device();```

Since there can be only one instance of M2MDevice, M2MDevice is a static class and it can be deleted using

`M2MDevice::delete_instance();`

Check the M2MDevice class documentation to see how you can configure the device object, as well as how to create appropriate Resources and assign values to them. 

####Security Object

There is a direct API in the M2MInterfaceFactory class to create a Security Object:

```static M2MSecurity *create_security(M2MSecurity::ServerType server_type);```

You can create a Bootstrap or normal mbed Device Server by passing the appropriate enum value.

Check the M2MSecurity class documentation to see how you can configure the security object, as well as how to create appropriate Resources and assign values to them.

####Creating a custom Object

As per the OMA LWM2M specification, the client must have defined Objects, under which it can create Object Instances. You can create M2MObject using this API from the M2MInterfaceFactory class:

```static M2MObject *create_object(const String &name);```

You need to pass the name of the Object that you would like to create (like ```Test```); this will create an object with that name in mbed DS.


####Configuring the Object

Once you have created an Object (whether OMA specific or custom), you can configure various parameters in that object so that they can be controlled or modified to affect communication with mbed DS.

Here, we discuss a few of the most important parameters, which you must configure properly in order to work with the objects.

#####Setting Operation Mode

You can set the objects' operation mode so that they can handle GET, PUT, POST, DELETE or a combination of these requests coming from mbed DS.

The API that sets the operation mode (present in the M2MBase class) is

```virtual void set_operation(M2MBase::Operation operation);```

#####Setting Observable Mode

You can set the object to be an observing resource. This can be done by setting the observable mode using the following API:

```virtual void set_observable(bool observable);```

#####Setting CoAP content type

Currently the only available content type is the OMA TLV type. The OMA TLV type works only for objects with a numeric value. For example, if you're creating a custom object it must be of a numeric type like ```100```; only then will the CoAP TLV type work for the custom object. 

If you want your object to support the TLV type, set the object's CoAP content type as ```99```:

```virtual void set_coap_content_type(const uint8_t content_type);```

Later we will introduce support for the JSON content types.

##How to create and configure Object Instances

The M2MObjectInstance class is derived from the M2MBase class, so all the public methods can be used from M2MObjectInstance and its derived classes.

###Creating OMA defined ObjectInstance

####Device ObjectInstance

Since there can only be one instance for the Device Object , the ObjectInstance is automatically created when creating M2MDevice Object.

####Security ObjectInstance

Since there can only be one instance for the Device Object, the Object Instance is automatically created when creating M2MDevice Object based on a selected server type.

####Creating custom object instances

As per the OMA LWM2M specification, the client must have created Object Instances under Objects, which will eventually contain Resources.

You can create an M2M Object Instance using this API from the M2MObject class:

``` M2MObject::create_object_instance(uint16_t instance_id);```

You can pass an Object Instance ID to create appropriate Object Instances. Normally, Object Instances will start from ```0``` and increment. So the Object Instance structure on the mbed DS side would look like 

```Object/Object Instance ID```

You need to pass the name of the Object Instance ID that you would like to create (like ```0```); this will create an object like ```/Test/0``` in mbed DS.

###Configuring the Object Instance

Once you have created an Object Instance (whether OMA-specific or custom), you can configure various parameters in that object so that they can be controlled or modified to affect communication with mbed DS.

Here, we discuss a few of the most important parameters, which you must configure properly in order to work with the object instances.

####Setting operation mode

You can set the Object Instances' operation mode so that they can handle GET, PUT, POST, DELETE or a combination of these requests coming from mbed DS.

The API that can be used to set the operation mode (present in the M2MBase class) is:

```virtual void set_operation(M2MBase::Operation operation);```

####Setting observable mode

You can set the object to be an observing resource. 

This can be done by setting the observable mode using the following API:

```virtual void set_observable(bool observable);```

####Setting CoAP content type

Currently the only available content type is the OMA TLV type. The OMA TLV type works only for objects with a numeric value. For example, if you're creating a custom object it must be of a numeric type like ```100```; only then still the CoAP TLV type work for the custom object. 

If you want your object to support the TLV type, set the object's CoAP content type as ```99```:

```virtual void set_coap_content_type(const uint8_t content_type);```

Later we will introduce support for JSON content types.

Apart from this, there are multiple APIs that provides getter and remove functions for Object Instances in the M2MObjectInstance class; check the API documentation for their usage. 

##How to create and configure Resources and Resource Instances

The M2MResource class is derived from the M2MResourceInstance, which in turn is derived from the M2MBase class, so all the public methods can be used from M2MResource or M2MResourceInstance and their derived classes.

###Creating OMA-defined Resources

####Device Object Resources

There are direct APIs to create and set values for the Device Resources.

You can create the required Resource and set values based on their data types.

For Resources that take string values you can create and set the Resource using the following API:

```M2MResource* create_resource(DeviceResource resource, const String &value);```

For Resources that take integer values you can create and set the Resource using the following API:

```M2MResource* create_resource(DeviceResource resource, uint32_t value);```

There are a few resources that can have multiple instances. To create these resources, use these APIs:

```M2MResourceInstance* create_resource_instance(DeviceResource resource, uint32_t value,uint16_t instance_id); ```

Where ```instance_id``` is the Resource Instance ID, like ```/3/0/11/0```.

Check M2MDevice's API documentation to find which enums are supported for integer or string value types.

There are other APIs in the M2MDevice class, which you can use to set, remove and modify new values for the resources.

####Security Object Resources

There are direct APIs to create and set values for the Security Resources, based on their data types.

Most of the mandatory resources are created automatically when you create M2MSecurity object. You can set their values using the following APIs:

- For resources that take integer values:

	```bool set_resource_value(SecurityResource resource,uint32_t value);```

- For resources that take string values:

	```bool set_resource_value(SecurityResource resource,const String &value);```

- For resources that take binary values, like setting public keys and certificates:

	```bool set_resource_value(SecurityResource resource,onst uint8_t *value,const uint16_t length);```

You can create and set values for Resources that are not mandatory, and which take an integer value, using this API:

```M2MResource* create_resource(SecurityResource resource, uint32_t value);```

Check M2MSecurity's API documentation to find which enums are supported for integer, string or uint8_t* value types.

There are more APIs in the M2MSecurity class that you can use to set, remove and modify Resource values.

####Creating custom Resources

As per the OMA LWM2M specification, the client must have Resources under Object Instances which belong to Objects.

You can create different types of Resources or Resource Instances.

There are two types of Resources:

- M2MResource: a resource with a unique instance, like ```/Test/0/Resource```.

- M2MResourceInstance: a resource with multiple instances, like```/Test/0/Resource/0```, ```/Test/0/Resource/1```.

Apart from these two types, you can create two types of Resource and Resource Instances:

- Static: Resource and Resource Instances whose value doesn't change over time.

- Dynamic: Resource and Resource Instances whose value can change. These resources can be made observable.

___Creating dynamic and static single-instance Resources___

As per the OMA LWM2M specification, the client must have created Resources under Object Instance. You can create M2MResource from the M2MObjectInstance class.

- You can create a single-instance Resource with a static value using this API:

	``` 
	M2MResource* create_static_resource(const String &resource_name,
	                                    const String &resource_type,
	                                    M2MResourceInstance::ResourceType type,
                                            const uint8_t *value,
                                            const uint8_t value_length,
                                            bool multiple_instance = false);
        ```

- You can create a single-instance Resource with a dynamic value, which can be set later on, using this API:

	```
	M2MResource* create_dynamic_resource(const String &resource_name,
	                                     const String &resource_type,
	                                     M2MResourceInstance::ResourceType type,
                                             bool observable,
                                             bool multiple_instance = false);
        ```

These APIs take different parameters, which you can see in their documentation.

___Creating dynamic and static Resource Instances___

As per the OMA LWM2M specification, the client must have created Resource Instances under Resources.
You can create M2MResourceInstance from the M2MObjectInstance class.

- You can create a Resource Instance with a static value using this API:

	```
	M2MResourceInstance* create_static_resource_instance(const String &resource_name,
							     const String &resource_type,
							     M2MResourceInstance::ResourceType type,
                                                             const uint8_t *value,
                                                             const uint8_t value_length,
                                                             uint16_t instance_id);
        ```

- You can create a Resource Instance with a dynamic value, which can be set later on, using this API:

	```
	M2MResourceInstance* create_dynamic_resource_instance(const String &resource_name,
							      const String &resource_type,
							      M2MResourceInstance::ResourceType type,
                                                              bool observable,
                                                              uint16_t instance_id);
        ```

These APIs take different parameters, which you can see in their documentation.

####Configuring the Resource and Resource Instance

Once you have created a Resource or Resource Instance (whether OMA-specific or custom), you can configure various parameters so they can be controlled or modified to affect communication with mbed DS.

Here, we discuss a few of the most important parameters that you must configure properly in order to work with the Resource and Resource Instance.

#####Setting Operation Mode

You can set the Resource or Resource Instance operation mode so that they can handle GET, PUT, POST, DELETE or a combination of these requests coming from mbed DS.

The API that can be used to set the operation mode (present in the M2MBase class) is

```virtual void set_operation(M2MBase::Operation operation);```

#####Setting the value of a dynamic Resource or Resource Instance

You can set the value of a dynamic Resource or Resource Instance so that they can be sent to mbed DS using GET requests.

The API used to set the values (present in the M2MResourceInstance class) is

```virtual bool set_value(const uint8_t *value, const uint32_t value_length);```

#####Setting an executable function

For Dynamic Resources, you can pass a function pointer to the Resource or Resource Instance, which will get executed when mbed DS calls a POST method on that resource. The Resource or Resource Instance must support the POST operation mode for this feature to work.

You can pass the function pointer using this API:

```virtual void set_execute_function(execute_callback callback);```

Apart from this, there are multiple APIs that provide getter and remove functions for Resource and Resource Instances in the M2MResource and M2MResourceInstance classes. Check the API documentation for their usage. 

## API documentation

You can generate Doxygen API documentation for these APIs from a doxy file in the `doxygen` folder. You need to run the `doxygen` command from the `doxygen/` folder; it will generate a `docs` folder at the API source directory root level, where you can find the detailed documentation for each API.
