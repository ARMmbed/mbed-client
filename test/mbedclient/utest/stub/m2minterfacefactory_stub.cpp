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
#include "m2minterfacefactory_stub.h"

M2MInterface* M2MInterfaceFactory::create_interface(M2MInterfaceObserver &observer,
                                                    const String &endpoint_name,
                                                    const String &endpoint_type,
                                                    const int32_t life_time,
                                                    const uint16_t listen_port,
                                                    const String &domain,
                                                    M2MInterface::BindingMode mode,
                                                    M2MInterface::NetworkStack stack,
                                                    const String &context_address)
{

}

M2MSecurity* M2MInterfaceFactory::create_security(M2MSecurity::ServerType server_type)
{

}

M2MServer* M2MInterfaceFactory::create_server()
{

}

M2MDevice* M2MInterfaceFactory::create_device()
{

}

M2MFirmware* M2MInterfaceFactory::create_firmware()
{

}

M2MObject* M2MInterfaceFactory::create_object(const String &name)
{

}
