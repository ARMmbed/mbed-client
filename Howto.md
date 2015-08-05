#How to create Object , Object Instances and Resources for the client

This section explains how to create different types of Objects, Object Instances and Resources for the client as per the OMA LWM2M specifications.

In order for client  to communicate the resources to mDS, it needs to create Objects, Object Instances and Resources. 
This can be done very easily with the Client C++ APIs where client can define its resources in a similar structure as defined in the LWM2M specification.
This quick guide will explain how you can create and configure these resources using the C++ API.

##How to create and configure Objects

M2MObject class is derived out of M2MBase class so all the public methods can be used from M2MObject and its derived classes.

###Creating OMA defined Objects

####Device Object.
There is a direct API to create Device Object using following API from M2MInterfaceFactory class

```static M2MDevice *create_device();```

Since there can be only one instance of M2MDevice , M2MDevice is a static class and it can be deleted using

`M2MDevice::delete_instance();`

Check M2MDevice class documentation to see how you can configure the device object as well as how to create appropriate Resources and assign value to them. 

####Security Object.
There is a direct API to create Security  Object using following API from M2MInterfaceFactory class
```static M2MSecurity *create_security(M2MSecurity::ServerType server_type);```

You can create a Bootstrap or normal LWM2M server by passing appropriate enum value.

Check M2MSecurity class documentation to see how you can configure the security object as well as how to create appropriate Resources and assign value to them.

####Creating custom object
As per the OMA LWM2M specification, client must have defined Objects under which it can create Object Instances.
You can create M2MObject using this API from M2MInterfaceFactory class
```static M2MObject *create_object(const String &name);```


You need to pass the name of the Object that you would like to create like ```Test``` and this would create an object like ```/Test``` in the mDS to be used.


####Configuring the Object

Once you have created an Object (whether OMA specific or Custom), you can configure various parameters in that object so that they can be controlled or modified to commnicate with the mDS accordingly.
Here, we discuss few most important parameters that you must configure properly in order to work around with the objects.

#####Setting Operation Mode
You can set the Object operation mode so that they can handle GET, PUT, POST, DELETE or combination of these requests coming from mDS accordingly.
The API that can be used to set the operation mode (present in M2MBase class) is

```virtual void set_operation(M2MBase::Operation operation);```

#####Setting Observable Mode
You can set the Object to be an observing resource. 
This can be done by setting the observable mode accordingly using following API
```virtual void set_observable(bool observable);```


#####Setting CoAP Content Type
Currently only OMA TLV type is supported for the content type , which we have defined to be of value ```99``` . The OMA TLV type works only for objects which are numeric in value , for eg: the custom object that you would like to create must be of numeric type like ```100```, only then the CoAP TLV type will work for the custom object. 
Later we will introduce JSON support for Content type.
You can set the CoAP content typs for the object as ```99```, if you want TLV support for your object.

```virtual void set_coap_content_type(const uint8_t content_type);```


##How to create and configure Object Instances

M2MObjectInstance class is derived out of M2MBase class so all the public methods can be used from M2MObjectInstance and its derived classes.

###Creating OMA defined ObjectInstance

####Device ObjectInstance.
Since there can only be one instance for the Device Object , the ObjectInstance is automatically created when creating M2MDevice Object.

####Security ObjectInstance.
Since there can only be one instance for the Device Object , the ObjectInstance is automatically created when creating M2MDevice Object based on selected server type. 

####Creating custom object instances
As per the OMA LWM2M specification, client must have created ObjectInstances under Objects , which will eventually contain Resources.
You can create M2MObjectInstance using this API from M2MObject class

``` M2MObject::create_object_instance(uint16_t instance_id);```


You can pass Object Instance ID to create appropriate Object Instances, normally the Object Instances will start from ```0``` and increment accordingly. So the Object Instance structure on mDS side would look like ```Object/Object Instance ID```


You need to pass the name of the ObjectInstance ID that you would like to create like ```0``` and this would create an object like ```/Test/0``` in the mDS to be used.


###Configuring the ObjectInstance

Once you have created an ObjectInstance (whether OMA specific or Custom), you can configure various parameters in that object so that they can be controlled or modified to commnicate with the mDS accordingly.
Here, we discuss few most important parameters that you must configure properly in order to work around with the object instance.

####Setting Operation Mode
You can set the ObjectInstance operation mode so that they can handle **GET**, **PUT**, **POST**, **DELETE** or combination of these requests coming from mDS accordingly.
The API that can be used to set the operation mode (present in M2MBase class) is

```virtual void set_operation(M2MBase::Operation operation);```


####Setting Observable Mode
You can set the Object to be an observing resource. 
This can be done by setting the observable mode accordingly using following API

```virtual void set_observable(bool observable);```


####Setting CoAP Content Type
Currently only OMA TLV type is supported for the content type , which we have defined to be of value ```99```. The OMA TLV type works only for objects which are numeric in value , for eg: the custom object that you would like to create must be of numeric type like ```100```, only then the CoAP TLV type will work for the custom object. 
Later we will introduce JSON support for Content type.
You can set the CoAP content typs for the object as ```99```, if you want TLV support for your object.

```virtual void set_coap_content_type(const uint8_t content_type);```


Apart from this, there are multiple APIs which provides getter and remove functions for ObjectInstances present in M2MObjectInstance class, check the API documentation for their required usage. 

##How to create and configure Resources/ResourceInstances

M2MResource class is derived out of M2MResourceInstance which in turn is derived out of M2MBase class so all the public methods can be used from M2MResource/M2MResourceInstance and its derived classes.

###Creating OMA defined Resources

####Device Object Resources.
There are direct APIs to create and set values to the Device Resources. 
You can create the required Resource and set values based on their data types.

For Resources , which take the values as String you can create and set the Resource using following API.

```M2MResource* create_resource(DeviceResource resource, const String &value);```


For Resources , which take the values as Integer you can create and set the Resource using following API.

```M2MResource* create_resource(DeviceResource resource, uint32_t value);```


Their are few resources which can have multiple instances, for creating such resources , use these APIs.

```M2MResourceInstance* create_resource_instance(DeviceResource resource, uint32_t value,uint16_t instance_id); ```


where ```instance_id``` will be the resource instance id like ```/3/0/11/0```


Check the API documentation of M2MDevice to find which enums are supported for Integer or String value types.
There are more APIs in M2MDevice class which you can use to set, remove and modify new values to the resources.

####Security Object Resources.
There are direct APIs to create and set values to the Security Resources. 
You can create the required Resource and set values based on their data types.

For Resources which are not mandatory, are those which take in value as Integer, you can create and set value using this API .

```M2MResource* create_resource(SecurityResource resource, uint32_t value);```


Most of the mandatory resources are created automatically when you create M2MSecurity object.
You can set the values of those resources by using following APIs.

For resources, which take in Integer values, use these APIs.

```bool set_resource_value(SecurityResource resource,uint32_t value);```


For resources, which take in String values, use these APIs.

```bool set_resource_value(SecurityResource resource,const String &value);```


For resources, like setting public keys, certificates which take in binary values, use these APIs.

```bool set_resource_value(SecurityResource resource,onst uint8_t *value,const uint16_t length);```


Check the API documentation of M2MSecurity to find which enums are supported for Integer,String or uint8_t* value types.
There are more APIs in M2MSecurity class which you can use to set, remove and modify new values to the resources.

####Creating custom resources
As per the OMA LWM2M specification, client must have created Resources under ObjectInstances which belongs to Objects.

You can create different types of Resources or ResourceInstances.

There are 2 types of Resources

- M2MResource - Resouce with unique instance, like ```/Test/0/Resource```
-  M2MResourceInstance - Resources with multiple instances ```/Test/0/Resource/0```, ```/Test/0/Resource/0```


Apart from these 2 types, you can create 2 types of Resource/ResourceInstances

- Static  - Resource/ResourceInstances, whose value doesn't change over time.
- Dynamic - Resource/ResourceInstances whose value can change and these resources can be made observable.

We will first explain how to create different types of **Resource**

As per the OMA LWM2M specification, client must have created Resources under ObjectInstance.

You can create M2MResource from M2MObjectInstance class.

You can create a static resource using this API

```M2MResource* create_static_resource(const String &resource_name,const String &resource_type,M2MResourceInstance::ResourceType type,
                                        const uint8_t *value,const uint8_t value_length,bool multiple_instance = false);```


The API takes in different parameters which you can check in API documentation, using this API you can create a single instance Resource with static value.

You can create a dynamic resource using this API

```M2MResource* create_dynamic_resource(const String &resource_name,const String &resource_type,M2MResourceInstance::ResourceType type,
                                        bool observable,bool multiple_instance = false);```


The API takes in different parameters which you can check in API documentation, using this API you can create a single instance Resource having a dynamic value, which can be set later on.

We will now see how to create **Resources with instances**
As per the OMA LWM2M specification, client must have created Resource instances under Resources.
You can create M2MResourceInstance from M2MObjectInstance class.

You can create a static resource instance using this API

```M2MResourceInstance* create_static_resource_instance(const String &resource_name,const String &resource_type,M2MResourceInstance::ResourceType type,
                                                         const uint8_t *value,const uint8_t value_length,uint16_t instance_id);```


The API takes in different parameters which you can check in API documentation, using this API you can create ResourceInstance with static value.

You can create a dynamic resource instance using this API

```M2MResourceInstance* create_dynamic_resource_instance(const String &resource_name,const String &resource_type,M2MResourceInstance::ResourceType type,
                                                          bool observable,uint16_t instance_id);```


The API takes in different parameters which you can check in API documentation, using this API you can create ResourceInstance having a dynamic value, which can be set later on.


####Configuring the Resource/ResourceInstance

Once you have created  Resource/ResourceInstance (whether OMA specific or Custom), you can configure various parameters so they can be controlled or modified to commnicate with the mDS accordingly.
Here, we discuss few most important parameters that you must configure properly in order to work around with the Resource/ResourceInstance.

#####Setting Operation Mode
You can set the Resource/ResourceInstance operation mode so that they can handle **GET**, **PUT**, **POST**, **DELETE** or combination of these requests coming from mDS accordingly.
The API that can be used to set the operation mode (present in M2MBase class) is

```virtual void set_operation(M2MBase::Operation operation);```


Setting Value to the dynamic Resource/ResourceInstance
You can set the value of dynamic Resource/ResourceInstance so that those values can be sent to the mDS based on GET requests.
The API that can be used to set the operation mode (present in M2MResourceInstance class) is

```virtual bool set_value(const uint8_t *value, const uint32_t value_length);```


#####Setting Executable Function
For Dynamic Resources, you can pass the function pointer to the Resource/ResourceInstance, which will get executed when the mDS server calls POST method on that resource. Given the condition
that Resource/ResourceInstance supports POST operation mode.
You can pass the function pointer using this API

```virtual void set_execute_function(execute_callback callback);```


Apart from this, there are multiple APIs which provides getter and remove functions for Resource/ResourceInstances present in M2MResource/M2MResourceInstance class, check the API documentation for their required usage. 

## API documentation

You can generate Doxygen API documentation for these APIs from a doxy file which is present in the `doxygen` folder . You need to run `doxygen` command from the `doxygen/` folder and it will generate a `docs` folder at API source directory root level where you can find the detailed documentation for each API.
