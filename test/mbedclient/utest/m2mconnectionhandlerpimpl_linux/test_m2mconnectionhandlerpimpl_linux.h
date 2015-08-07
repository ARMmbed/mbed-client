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
#ifndef TEST_M2M_CONNECTION_HANDLER_PIMPL_LINUX_H
#define TEST_M2M_CONNECTION_HANDLER_PIMPL_LINUX_H

#include "mbed-client-linux/m2mconnectionhandlerpimpl.h"

class M2MConnection_TestObserver;

class Test_M2MConnectionHandlerPimpl_linux
{
public:
    Test_M2MConnectionHandlerPimpl_linux();

    virtual ~Test_M2MConnectionHandlerPimpl_linux();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_start_listening_for_data();

    void test_data_receive();

    void test_stop_listening();

    void test_sendToSocket();

    void test_receiveFromSocket();

    M2MConnectionHandlerPimpl* handler;
    M2MConnection_TestObserver *observer;
};

#endif // TEST_M2M_CONNECTION_HANDLER_PIMPL_LINUX_H
