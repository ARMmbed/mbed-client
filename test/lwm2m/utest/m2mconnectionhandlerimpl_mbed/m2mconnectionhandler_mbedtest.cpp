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
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandler_mbed.h"

TEST_GROUP(M2MConnectionHandler_mbed)
{
  Test_M2MConnectionHandler_mbed* handler;

  void setup()
  {
    handler = new Test_M2MConnectionHandler_mbed();
  }
  void teardown()
  {
    delete handler;
  }
};


TEST(M2MConnectionHandler_mbed, create)
{
    CHECK(handler != NULL);
}

TEST(M2MConnectionHandler_mbed, test_bind_connection)
{
    handler->test_bind_connection();
}

TEST(M2MConnectionHandler_mbed, test_resolve_server_address)
{
    handler->test_resolve_server_address();
}

TEST(M2MConnectionHandler_mbed, test_send_data)
{
    handler->test_send_data();
}

TEST(M2MConnectionHandler_mbed, test_start_listening_for_data)
{
    handler->test_start_listening_for_data();
}

TEST(M2MConnectionHandler_mbed, test_stop_listening)
{
    handler->test_stop_listening();
}

TEST(M2MConnectionHandler_mbed, test_sendToSocket)
{
    handler->test_sendToSocket();
}

TEST(M2MConnectionHandler_mbed, test_receiveFromSocket)
{
    handler->test_receiveFromSocket();
}
