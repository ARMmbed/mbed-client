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
#include "m2mconnectionhandlerpimpl_stub.h"
#include "mbed-client-linux/m2mconnectionhandlerpimpl.h"
#include "m2mconnectionsecurity.h"
#include "common_stub.h"

int m2mconnectionhandlerpimpl_stub::int_value;
uint16_t m2mconnectionhandlerpimpl_stub::uint_value;
bool m2mconnectionhandlerpimpl_stub::bool_value;

void m2mconnectionhandlerpimpl_stub::clear()
{
    int_value = -1;
    uint_value = 0;
    bool_value = false;
}

M2MConnectionHandlerPimpl::M2MConnectionHandlerPimpl(M2MConnectionHandler* base, M2MConnectionObserver &observer,
                                                     M2MConnectionSecurity* sec,
                                                     M2MInterface::BindingMode,
                                                     M2MInterface::NetworkStack)
:_observer(observer)
{
    if( sec ){
        delete sec;
        sec = NULL;
    }
}

M2MConnectionHandlerPimpl::~M2MConnectionHandlerPimpl()
{
}

bool M2MConnectionHandlerPimpl::bind_connection(const uint16_t)
{
    return m2mconnectionhandlerpimpl_stub::bool_value;
}

bool M2MConnectionHandlerPimpl::resolve_server_address(const String&,
                                                  const uint16_t,
                                                  M2MConnectionObserver::ServerType,
                                                  const M2MSecurity* )
{
    return m2mconnectionhandlerpimpl_stub::bool_value;
}

bool M2MConnectionHandlerPimpl::start_listening_for_data()
{
    return m2mconnectionhandlerpimpl_stub::bool_value;
}

bool M2MConnectionHandlerPimpl::send_data(uint8_t *,
                                     uint16_t ,
                                     sn_nsdl_addr_s *)
{
    return m2mconnectionhandlerpimpl_stub::bool_value;
}

void M2MConnectionHandlerPimpl::stop_listening()
{
}

int M2MConnectionHandlerPimpl::sendToSocket(const unsigned char *, size_t ){
    return m2mconnectionhandlerpimpl_stub::int_value;
}

int M2MConnectionHandlerPimpl::receiveFromSocket(unsigned char *buf, size_t len){
    return m2mconnectionhandlerpimpl_stub::int_value;
}
