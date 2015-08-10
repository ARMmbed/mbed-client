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
#ifndef TEST_M2M_CONNECTION_HANDLER_PIMPL_MBED_H
#define TEST_M2M_CONNECTION_HANDLER_PIMPL_MBED_H

#include "mbed-client-mbed-os/m2mconnectionhandlerpimpl.h"

class TestObserver;

class Test_M2MConnectionHandlerPimpl_mbed
{
public:
    Test_M2MConnectionHandlerPimpl_mbed();

    virtual ~Test_M2MConnectionHandlerPimpl_mbed();

    void test_constructor();

    void test_bind_connection();

    void test_resolve_server_address();

    void test_send_data();

    void test_start_listening_for_data();

    void test_stop_listening();

    void test_sendToSocket();

    void test_receiveFromSocket();

    //Special case: test privates also
    void test_send_handler();
    void test_receive_handler();
    void test_receive_handshake_handler();
    void test_dns_handler();
    void test_error_handler();

    M2MConnectionHandlerPimpl* handler;
    TestObserver *observer;
};

#endif // TEST_M2M_CONNECTION_HANDLER_PIMPL_MBED_H
