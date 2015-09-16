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
#include "test_m2mconnectionhandlerfactory.h"
#include "m2mconnectionhandler_stub.h"
#include "m2mconnectionobserver.h"


class TestObserver : public M2MConnectionObserver {

public:
    TestObserver(){}
    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){}

    void socket_error(uint8_t error_code){}

    void address_ready(const M2MConnectionObserver::SocketAddress &,
                       M2MConnectionObserver::ServerType,
                       const uint16_t){}

    void data_sent(){}
};

Test_M2MConnectionHandlerFactory::Test_M2MConnectionHandlerFactory()
{

}

Test_M2MConnectionHandlerFactory::~Test_M2MConnectionHandlerFactory()
{

}

void Test_M2MConnectionHandlerFactory::test_createConnectionHandler()
{
    TestObserver obs;
    M2MConnectionHandler* obj = M2MConnectionHandlerFactory::createConnectionHandler(obs, M2MInterface::NOT_SET,M2MInterface::LwIP_IPv4);
    CHECK(obj != NULL );
    delete obj;
}
