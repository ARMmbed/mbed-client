/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_RESOURCE_H
#define M2M_RESOURCE_H

#include "m2mbase.h"
#include "functionpointer.h"

/**
 *  @brief M2MResource.
 *  This class is the base class for LWM2M Resources based on which all defined
 *  LWM2M resource model can be created.
 */
typedef FP1<void,void*> execute_callback;

class M2MResource : public M2MBase {

friend class M2MObjectInstance;

private: // Constructor and destructor are private
         // so that these objects can be created or
         // deleted only through function provided by M2MObjectInstance.
    /**
     * @brief Constructor for creating resource.
     * @param name, name of the resource
     * @param resource_type, Resource Type.
     * @param resource_mode, Static or Dynamic.
     * @param multiple_instance, Resource can have
     *        multiple instances.
     */
    M2MResource(const String &resource_name,
                const String &resource_type,
                M2MBase::Mode resource_mode,
                bool multiple_instances = false);

    // Prevents the use of default constructor.
    M2MResource();

    // Prevents the use of assignment operator.
    M2MResource& operator=( const M2MResource& /*other*/ );

    // Prevents the use of copy constructor
    M2MResource( const M2MResource& /*other*/ );

    /**
     * Destructor
     */
    virtual ~M2MResource();

public:

    /**
     * @brief Returns object type
     * @return BaseType
     */
    virtual M2MBase::BaseType base_type() const;

    bool supports_multiple_instances() const;

    /**
     * @brief Parses the received query for notification
     * attribute.
     * @return true if required attributes are present else false.
     */
    virtual bool handle_observation_attribute(char *&query);

    virtual void set_execute_function(execute_callback callback);

    void execute(void *arguments);

private:

    bool                    _has_multiple_instances;
    execute_callback        _execute_callback;

    friend class Test_M2MResource;
    friend class Test_M2MObjectInstance;
    friend class Test_M2MObject;
    friend class Test_M2MDevice;
    friend class Test_M2MSecurity;
    friend class Test_M2MServer;
    friend class Test_M2MNsdlInterface;
};

#endif // M2M_RESOURCE_H

