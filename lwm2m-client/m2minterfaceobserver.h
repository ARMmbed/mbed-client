/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_INTERFACE_OBSERVER_H
#define M2M_INTERFACE_OBSERVER_H

#include "m2minterface.h"

//FORWARD DECLARATION
class M2MServer;

/**
 * @brief M2MInterfaceObserver
 * This is an observer class which updates the calling application about
 * various events associated with various Interface operations.
 * Also, it informs about various errors  which can occur during any of the above
 * operations.
 */
class M2MInterfaceObserver {

public:

    /**
     * @brief Callback informing that the bootstap has been done successfully.
     * @param server_object Server object which contains information fetched
     * about LWM2M server from the bootstrap server, this object can be used
     * to register to the LWM2M server. The object ownership is passed.
     */
    virtual void bootstrap_done(M2MSecurity *server_object) = 0;

    /**
     * @brief Callback informing that the device object has been registered
     * successfully to the LWM2M server.
     * @param security_object Server object on which the device object is
     * registered.The object ownership is passed.
     * @param server_object, Object containing information about LWM2M server.
     * Client maintains the object.
     */
    virtual void object_registered(M2MSecurity *security_object, const M2MServer &server_object) = 0;

    /**
     * @brief Callback informing that the device object has been unregistered
     * successfully to the LWM2M serevr.
     * @param server_object Server object on which the device object is
     * unregistered.The object ownership is passed.
     */
    virtual void object_unregistered(M2MSecurity *server_object) = 0;

    /**
     * @brief Callback informing that the device object registration has been
     * updated successfully on the LWM2M serevr.
     * @param server_object Server object on which the device object registration
     * updated. The object ownership is passed.
     * @param server_object, Object containing information about LWM2M server.
     * Client maintains the object.
     */
    virtual void registration_updated(M2MSecurity *security_object, const M2MServer & server_object) = 0;

    /**
     * @brief Callback informing that there is some error during operation.
     * @param error Error code informing about the error that occured.
     */
    virtual void error(M2MInterface::Error error) = 0;

    /**
     * @brief Callback informing that the value of the resource object is updated by server.
     * @param base Object whose value is updated.
     * @param type Type of object.
     */
    virtual void value_updated(M2MBase *base, M2MBase::BaseType type) = 0;
};

#endif // M2M_INTERFACE_OBSERVER_H
