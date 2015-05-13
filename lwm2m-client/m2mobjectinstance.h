/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_OBJECT_INSTANCE_H
#define M2M_OBJECT_INSTANCE_H

#include "m2mvector.h"
#include "m2mbase.h"

//FORWARD DECLARATION
class M2MResource;
typedef Vector<M2MResource *> M2MResourceList;

/**
 *  @brief M2MObject.
 *  This class is the base class for LWM2M Objects based on which all defined
 *  LWM2M objects model can be created. This class will also hold all resources
 *  instances associated with the given object.
 */

class M2MObjectInstance : public M2MBase {

friend class M2MObject;

private: // Constructor and destructor are private
         // so that these objects can be created or
         // deleted only through function provided by M2MObject.

    /**
     * @brief Constructor
     * @param name, name of the object
     */
    M2MObjectInstance(const String &object_name);


    // Prevents the use of default constructor.
    M2MObjectInstance();

    // Prevents the use of assignment operator.
    M2MObjectInstance& operator=( const M2MObjectInstance& /*other*/ );

    // Prevents the use of copy constructor
    M2MObjectInstance( const M2MObjectInstance& /*other*/ );

    /**
     * Destructor
     */
    virtual ~M2MObjectInstance();

public:


    /**
     * @brief Creates static resource for a given object LWM2M Inteface using which
     * client can respond to server's GET methods with the provided value.
     * @param resource_name, Name of the resource.
     * @param resource_type, Type of the resource.
     * @param value, pointer to the value of the resource.
     * @param value_length, length to the value in pointer.
     * @param multiple_instance, Resource can have
     *        multiple instances,default is false.
     * @return M2MResource, Resource to manage other LWM2M operations.
     */
    M2MResource* create_static_resource(const String &resource_name,
                                        const String &resource_type,
                                        const uint8_t *value,
                                        const uint8_t value_length,
                                        bool multiple_instance = false);

    /**
     * @brief Creates dynamic resource for a given object LWM2M Inteface using which
     * client can respond to different queries from server (GET,PUT etc).Also,
     * this type of resource is observable and will carry callbacks.
     * @param resource_name, Name of the resource.
     * @param resource_type, Type of the resource.
     * @param observable, whether resource is observable or not.
     * @param multiple_instance, Resource can have
     *        multiple instances, default is false.
     * @return M2MResource, Resource to manage other LWM2M operations.
     */
    M2MResource* create_dynamic_resource(const String &resource_name,
                                         const String &resource_type,
                                         bool observable,
                                         bool multiple_instance =false);

    /**
     * @brief Removes the resource with given name.
     * @param name, Name of the resource to be removed
     * @param instance_id, Instance ID of resource to be removed, default is 0
     * @return True if removed else false.
     */
    virtual bool remove_resource(const String &name, uint16_t instance_id = 0);

    /**
     * @brief Returns resource with the given name
     * @param name, Name of the requested resource
     * @param instance_id, Instance ID of the requested resource, default is 0
     * @return Resource reference if found else NULL.
     */
    virtual M2MResource* resource(const String &name, uint16_t instance_id = 0) const;

    /**
     * @brief Returns list of resources
     * @return List of resources with the object.
     */
    virtual const M2MResourceList& resources() const;

    /**
     * @brief Returns total number of resources with the object.
     * @return Total count of the resources.
     */
    virtual uint16_t resource_count() const;

    /**
     * @brief Returns total instance of single resource.
     * @param resource, Name of the resource.
     * @return Total count of the resources.
     */
    virtual uint16_t resource_count(const String& resource) const;

    /**
     * @brief Returns object type
     * @return BaseType
     */
    M2MBase::BaseType base_type() const;

    /**
     * @brief Parses the received query for notification
     * attribute.
     * @return true if required attributes are present else false.
     */
    virtual bool handle_observation_attribute(char *&query);

private:

    void add_resource(M2MResource *res);

private:

    M2MResourceList     _resource_list; // owned

    friend class Test_M2MObjectInstance;
    friend class Test_M2MObject;
    friend class Test_M2MDevice;
    friend class Test_M2MSecurity;
    friend class Test_M2MServer;
    friend class Test_M2MNsdlInterface;
};

#endif // M2M_OBJECT_INSTANCE_H
