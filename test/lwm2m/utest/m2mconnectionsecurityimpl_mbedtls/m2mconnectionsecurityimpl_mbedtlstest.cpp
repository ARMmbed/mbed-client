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
#include "test_m2mconnectionsecurityimpl_mbedtls.h"


TEST_GROUP(M2MConnectionSecurityImpl_mbedtls)
{
  Test_M2MConnectionSecurityImpl* inst;

  void setup()
  {
    inst = new Test_M2MConnectionSecurityImpl();
  }
  void teardown()
  {
    delete inst;
  }
};

TEST(M2MConnectionSecurityImpl_mbedtls, Create)
{
    CHECK(inst != NULL);
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_constructor)
{
    inst->test_constructor();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_destructor)
{
    inst->test_destructor();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_reset)
{
    inst->test_reset();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_init)
{
    inst->test_init();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_connect)
{
    inst->test_connect();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_start_connecting_non_blocking)
{
    inst->test_start_connecting_non_blocking();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_continue_connecting)
{
    inst->test_continue_connecting();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_send_message)
{
    inst->test_send_message();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_read)
{
    inst->test_read();
}

