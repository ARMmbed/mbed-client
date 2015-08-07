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
#include <unistd.h>
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient();

    ~M2MLWClient();

    bool create_interface(int32_t param_count,
    					  const char *endpoint,
                          const char *resource_type,
                          const int32_t lifetime,
                          const uint16_t listen_port,
                          const char *domain,
                          const uint8_t binding_mode,
                          const uint8_t network_interface);

    bool create_bootstrap_object(const char *coap_bootstrap_address);

    bool create_register_object(const char *coap_register_address);

    bool test_bootstrap();

    bool create_device_object();

    bool create_device_object(M2MDevice::DeviceResource resource,
                              const char *value);

    bool create_device_object(M2MDevice::DeviceResource resource,
                              uint32_t value,
                              uint16_t instance_id);

    bool create_device_object(M2MDevice::DeviceResource resource,
                              uint32_t value);

    bool create_object(const char *name,bool new_instance);

    bool create_static_resource_string(const char *name,
                                       const char *value,
                                       bool multiple_instance,
                                       uint16_t object_instance = 0);

    bool create_static_resource_int(const char *name,
                                    uint32_t value,
                                    bool multiple_instance,
                                    uint16_t object_instance = 0);

    bool create_dynamic_resource(const char *name,
                                 bool observable,
                                 bool multiple_instance,
                                 uint16_t object_instance = 0);

    bool set_resource_value(const char *name,
                            int32_t value,
                            uint16_t object_instance = 0);

    bool set_resource_value(const char *name,
                            const char *value,
                            uint16_t object_instance = 0);

    bool create_static_resource_instance_string(const char *name,
                                                const char *value,
                                                bool multiple_instance,
                                                uint16_t object_instance = 0,
                                                uint16_t resource_instance = 0);

    bool create_static_resource_instance_int(const char *name,
                                             uint32_t value,
                                             bool multiple_instance,
                                             uint16_t object_instance = 0,
                                             uint16_t resource_instance = 0);

    bool create_dynamic_resource_instance(const char *name,
                                          bool observable,
                                          bool multiple_instance,
                                          uint16_t object_instance = 0,
                                          uint16_t resource_instance = 0);

    bool set_resource_instance_value(const char *name,
                                     int32_t value,
                                     uint16_t object_instance = 0,
                                     uint16_t resource_instance = 0);

    bool set_resource_instance_value(const char *name,
                                     const char *value,
                                     uint16_t object_instance = 0,
                                     uint16_t resource_instance = 0);

    bool create_object_instance();

    bool test_register();

    bool test_update_register(const uint32_t lifetime);

    bool test_unregister();

    void bootstrap_done(M2MSecurity *server_object);

    void object_registered(M2MSecurity *security_object, const M2MServer &server_object);

    void object_unregistered(M2MSecurity *server_object);

    void registration_updated(M2MSecurity *security_object, const M2MServer &server_object);

    void error(M2MInterface::Error error);

    void value_updated(M2MBase *base, M2MBase::BaseType type);

private:

    M2MInterface        *_interface;
    M2MSecurity         *_security;
    M2MSecurity         *_register_security;
    M2MDevice           *_device;
    M2MObject           *_object;
    bool                _bootstrapped;
    bool                _error;
    bool                _registered;
    bool                _unregistered;
    bool                _registration_updated;
};
