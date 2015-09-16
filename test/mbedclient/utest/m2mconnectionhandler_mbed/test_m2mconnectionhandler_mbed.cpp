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
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandler_mbed.h"
#include "m2mconnectionhandler.h"
#include "m2mconnectionhandlerpimpl_stub.h"
#include "m2mconnectionobserver.h"
#include "common_stub.h"

class TestObserver : public M2MConnectionObserver {

public:
    TestObserver(){}

    virtual ~TestObserver(){}

    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){}

    void socket_error(uint8_t error_code){}

    void address_ready(const M2MConnectionObserver::SocketAddress &,
                       M2MConnectionObserver::ServerType,
                       const uint16_t){}

    void data_sent(){}

    bool dataAvailable;
    bool error;
    bool addressReady;
    bool dataSent;
};

Test_M2MConnectionHandler_mbed::Test_M2MConnectionHandler_mbed()
{
    observer = new TestObserver();
    handler = new M2MConnectionHandler(*observer, NULL,M2MInterface::NOT_SET,M2MInterface::Uninitialized);
}

Test_M2MConnectionHandler_mbed::~Test_M2MConnectionHandler_mbed()
{
    delete handler;
    delete observer;
}

//void Test_M2MConnectionHandler_mbed::test_constructor()
//{
//    TestObserver obs;
//    M2MConnectionHandler impl = M2MConnectionHandler(obs,M2MInterface::Uninitialized);
//    CHECK(impl._socket_stack == SOCKET_STACK_UNINIT);

//    M2MConnectionHandler impl2 = M2MConnectionHandler(obs,M2MInterface::LwIP_IPv4);
//    CHECK(impl2._socket_stack == SOCKET_STACK_LWIP_IPV4);

//    M2MConnectionHandler impl3 = M2MConnectionHandler(obs,M2MInterface::LwIP_IPv6);
//    CHECK(impl3._socket_stack == SOCKET_STACK_LWIP_IPV6);

//    M2MConnectionHandler impl4 = M2MConnectionHandler(obs,M2MInterface::Reserved);
//    CHECK(impl4._socket_stack == SOCKET_STACK_RESERVED);

//    M2MConnectionHandler impl5 = M2MConnectionHandler(obs,M2MInterface::Nanostack_IPv6);
//    CHECK(impl5._socket_stack == SOCKET_STACK_NANOSTACK_IPV6);

//    M2MConnectionHandler impl6 = M2MConnectionHandler(obs,M2MInterface::Unknown);
//    CHECK(impl6._socket_stack == SOCKET_STACK_MAX);
//}

void Test_M2MConnectionHandler_mbed::test_bind_connection()
{
    m2mconnectionhandlerpimpl_stub::bool_value = true;
    CHECK( handler->bind_connection(7) == true);
}

void Test_M2MConnectionHandler_mbed::test_resolve_server_address()
{
    m2mconnectionhandlerpimpl_stub::bool_value = true;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, NULL) == true);
}

void Test_M2MConnectionHandler_mbed::test_send_data()
{
    m2mconnectionhandlerpimpl_stub::bool_value = true;
    CHECK( true == handler->send_data(NULL, 0 , NULL));
}

void Test_M2MConnectionHandler_mbed::test_start_listening_for_data()
{
    handler->start_listening_for_data();
}

void Test_M2MConnectionHandler_mbed::test_stop_listening()
{
    // Empty function to cover the cases
    handler->stop_listening();
}

void Test_M2MConnectionHandler_mbed::test_sendToSocket()
{
    const char buf[] = "hello";
    handler->sendToSocket((unsigned char *)&buf, 5);
}

void Test_M2MConnectionHandler_mbed::test_receiveFromSocket()
{
    unsigned char *buf = (unsigned char *)malloc(6);
    handler->receiveFromSocket(buf, 5);
    free(buf);
}
