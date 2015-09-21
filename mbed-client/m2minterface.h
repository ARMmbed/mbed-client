/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef M2M_INTERFACE_H
#define M2M_INTERFACE_H

#include <stdint.h>
#include "mbed-client/m2mvector.h"
#include "mbed-client/m2mconfig.h"
#include "mbed-client/functionpointer.h"

//FORWARD DECLARATION
class M2MSecurity;
class M2MObject;
class M2MInterfaceObserver;

typedef Vector<M2MObject *> M2MObjectList;
typedef FP callback_handler;

/**
 *  @brief M2MInterface.
 *  This class provides interface for handling all the mbed Client Interface operations
 *  defined in OMA LWM2M specifications.
 *  This includes Bootstrapping, Client Registration, Device Management &
 *  Service Enablement and Information Reporting.
 */

class M2MInterface {

public:

    /**
     * @brief Enum defining different kind of errors
     * which can occur during various client operations.
     */
    typedef enum {
        ErrorNone = 0,
        AlreadyExists,
        BootstrapFailed,
        InvalidParameters,
        NotRegistered,
        Timeout,
        NetworkError,
        ResponseParseFailed,
        UnknownError,
        MemoryFail,
        NotAllowed
    }Error;

    /**
     * @brief Enum defining different kind of binding
     * mode handled for client operations.
     */
    typedef enum {
        NOT_SET = 0,
        UDP = 0x01,
        UDP_QUEUE = 0x03,
        SMS = 0x04,
        SMS_QUEUE =0x06,
        UDP_SMS_QUEUE = 0x07,
        TCP = 0x08, //not real value, spec does not have one!
        TCP_QUEUE = 0x09 //not real value, spec does not have one!
    }BindingMode;

    /**
     * @brief Enum defining different kind of network
     * stack which can be used by mbed Client.
     */
    typedef enum {
        Uninitialized = 0,
        LwIP_IPv4,
        LwIP_IPv6,
        Reserved,
        Nanostack_IPv6,
        Unknown
    }NetworkStack;

public:

    virtual ~M2MInterface(){}

    /**
     * @brief Initiates bootstrapping of the client with the provided Bootstrap
     * server information.
     * NOTE: This API is not supported for developers!!
     * @param security_object, Security object which contains information
     * required for successful bootstrapping of the client.
     */
    virtual void bootstrap(M2MSecurity *security_object) = 0;

    /**
     * @brief Cancels on going bootstrapping operation of the client. If the client has
     * already successfully bootstrapped then this function deletes existing
     * bootstrap information from the client.
     * NOTE: This API is not supported for developers!!
     */
    virtual void cancel_bootstrap() = 0;

    /**
     * @brief Initiates registration of the provided Security object to the
     * corresponding LWM2M server.
     * @param security_object, Security object which contains information
     * required for registering to the LWM2M server.
     * If client wants to register to multiple LWM2M servers then it has call
     * this function once for each of LWM2M server object separately.
     * @param object_list, Objects which contains information.
     * which the client want to register to the LWM2M server.
     */
    virtual void register_object(M2MSecurity *security_object, const M2MObjectList &object_list) = 0;

    /**
     * @brief Updates or refreshes the client's registration on the LWM2M
     * server.
     * @param security_object, Security object from which the device object.
     * needs to update registration, if there is only one LWM2M server registered
     * then this parameter can be NULL.
     * @param lifetime, Lifetime for the endpoint client in seconds, if the same value
     * has to be passed then put the default value as 0.
     */
    virtual void update_registration(M2MSecurity *security_object, const uint32_t lifetime = 0) = 0;

    /**
     * @brief Unregisters the registered object from the LWM2M server.
     * @param security_object, Security object from which the device object
     * needs to be unregistered, if there is only one LWM2M server registered
     * then this parameter can be NULL.
     */
    virtual void unregister_object(M2MSecurity* security_object = NULL) = 0;

    /**
     * @brief Sets the function which will be called indicating client
     * is going to sleep when the Binding mode is selected with Queue mode.
     * @param callback, Function pointer which will be called when client
     * goes to seleep.
     */
    virtual void set_queue_sleep_handler(callback_handler handler) = 0;

};

#endif // M2M_INTERFACE_H
