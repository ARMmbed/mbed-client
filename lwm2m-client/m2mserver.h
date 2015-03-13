#ifndef M2M_SERVER_H
#define M2M_SERVER_H

#include "m2mobject.h"

// FORWARD DECLARATION
class M2MResource;

/**
 *  @brief M2MServer.
 *  This class represents interface for the Server Object model of LWM2M framework.
 *  This class will handle the server object and all its corresponding
 *  resources.
 */

class M2MServer : public M2MObject
{

friend class M2MInterfaceFactory;
friend class M2MNsdlInterface;

public:

    /**
     * @brief Enum defining all the resources associdated with
     * Server Object in LWM2M framework.
     */
    typedef enum {
        ShortServerID,
        Lifetime,
        DefaultMinPeriod,
        DefaultMaxPeriod,
        Disable,
        DisableTimeout,
        NotificationStorage,
        Binding,
        RegistrationUpdate
    }ServerResource;

private:

    /**
     * @brief Constructor
     */
    M2MServer();


    // Prevents the use of assignment operator.
    M2MServer& operator=( const M2MServer& /*other*/ );

    // Prevents the use of copy constructor
    M2MServer( const M2MServer& /*other*/ );

public:

    /**
     * @brief Destructor
     */
    virtual ~M2MServer();

    /**
     * @brief Creates a new resource for given resource enum.
     * @param resource, List of resource names which can set value using this function are
     *  'ShortServerID','Lifetime','DefaultMinPeriod','DefaultMaxPeriod','DisableTimeout',
     *  'NotificationStorage'.
     * @param value, Value to be set on the resource, in Integer format
     * @return M2MResource if created successfully else NULL.
     */
    M2MResource* create_resource(ServerResource resource, uint32_t value);

    /**
     * @brief Creates a new resource for given resource enum.
     * @param resource, List of resource names which can be created using this function are
     * 'Disable', 'RegistrationUpdate'
     * @return M2MResource if created successfully else NULL.
     */
    M2MResource* create_resource(ServerResource resource);

    /**
     * @brief Deletes the resource with the given resource enum.
     * It cannot delete the mandatory resources.
     * @param resource, Name of the resource to be deleted.
     * @return True if deleted else false.
     */
    bool delete_resource(ServerResource rescource);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource names which can set value using this function are
     * 'Binding'.
     * @param value, Value to be set on the resource, in String format
     * @return True if successfully set else false.
     */
    bool set_resource_value(ServerResource resource,
                            const String &value);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource names which can set value using this function are
     *  'ShortServerID','Lifetime','DefaultMinPeriod','DefaultMaxPeriod','DisableTimeout',
     *  'NotificationStorage'.
     * @param value, Value to be set on the resource, in Integer format
     * @return True if successfully set else false.
     */
    bool set_resource_value(ServerResource resource,
                            uint32_t value);
    /**
     * @brief Returns the value of the given resource enum, in String
     * @param resource, List of resource names which can return value using this function are
     * 'Binding'.
     * @return Value associated with that resource, if resourceis not valid it returns empty string.
     */
    String resource_value_string(ServerResource resource) const;

    /**
     * @brief Returns the value of the given resource name, in Integer
     * @param resource, List of resource names which can return value using this function are
     *  'ShortServerID','Lifetime','DefaultMinPeriod','DefaultMaxPeriod','DisableTimeout',
     *  'NotificationStorage'
     * @return Value associated with that resource, if resource is not valid it returns -1.
     */
    uint32_t resource_value_int(ServerResource resource) const;

    /**
     * @brief Returns if the resource instance with given resource enum exists or not
     * @param resource, resource enum.
     * @return True if at least one instance exists else false.
     */
    bool is_resource_present(ServerResource resource)const;

    /**
     * @brief Returns total number of resources for server object.
     * @return Total Number of resources.
     */
    uint16_t total_resource_count()const;

private:

    M2MResource* get_resource(ServerResource res) const;


private:

    M2MObjectInstance*    _server_instance;

    friend class Test_M2MServer;
};

#endif // M2M_SERVER_H
