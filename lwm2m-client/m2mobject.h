/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_OBJECT_H
#define M2M_OBJECT_H

#include "m2mvector.h"
#include "m2mbase.h"

//FORWARD DECLARATION
class M2MObjectInstance;
typedef Vector<M2MObjectInstance *> M2MObjectInstanceList;

/**
 *  @brief M2MObject.
 *  This class is the base class for LWM2M Objects based on which all defined
 *  LWM2M objects model can be created. This class will also hold all object
 *  instances associated with the given object.
 */

class M2MObject : public M2MBase {

friend class M2MInterfaceFactory;

protected :

    /**
     * @brief Constructor
     * @param name, name of the object
     */
    M2MObject(const String &object_name);


    // Prevents the use of default constructor.
    M2MObject();

    // Prevents the use of assignment operator.
    M2MObject& operator=( const M2MObject& /*other*/ );

    // Prevents the use of copy constructor
    M2MObject( const M2MObject& /*other*/ );

public:

    /**
     * @brief Destructor
     */
    virtual ~M2MObject();

    /**
     * @brief Creates new object instance for a given object LWM2M Inteface using which
     * client can respond to server's GET methods with the provided value.
     * @return M2MObjectInstance, Object instance to manage other LWM2M operations.
     */
    M2MObjectInstance* create_object_instance();

    /**
     * @brief Removes the object instance resource with given instance id.
     * @param instance_id, Instance ID of object instance to be removed, default is 0
     * @return True if removed else false.
     */
    virtual bool remove_object_instance(uint16_t instance_id = 0);

    /**
     * @brief Returns object instance with the given instance id
     * @param instance_id, Instance ID of the requested object instance id, default is 0
     * @return Object instance reference if found else NULL.
     */
    virtual M2MObjectInstance* object_instance(uint16_t instance_id = 0) const;

    /**
     * @brief Returns list of object instances
     * @return List of instances with the object.
     */
    virtual const M2MObjectInstanceList& instances() const;

    /**
     * @brief Returns total number of instances with the object.
     * @return Total count of the object instances.
     */
    virtual uint16_t instance_count() const;

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

    void add_object_instance(M2MObjectInstance *obj);

private:

    M2MObjectInstanceList     _instance_list; // owned
    uint16_t                  _instance_index;

friend class Test_M2MObject;
friend class Test_M2MInterfaceImpl;
friend class Test_M2MNsdlInterface;

};

#endif // M2M_OBJECT_H
