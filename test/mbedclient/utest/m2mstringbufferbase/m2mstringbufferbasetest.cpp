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
#include "test_m2mstringbufferbase.h"

TEST_GROUP(M2Mstringbufferbase)
{
  Test_M2Mstringbufferbase* stringbufferbase;

  void setup()
  {
    stringbufferbase = new Test_M2Mstringbufferbase();
  }
  void teardown()
  {
    delete stringbufferbase;
  }
};

TEST(M2Mstringbufferbase, Create)
{
    CHECK(stringbufferbase != NULL);
}

TEST(M2Mstringbufferbase, ensure_space)
{
    stringbufferbase->test_ensure_space();
}

TEST(M2Mstringbufferbase, append)
{
    stringbufferbase->test_append();
}

TEST(M2Mstringbufferbase, append_int)
{
    stringbufferbase->test_append_int();
}

TEST(M2Mstringbufferbase, find_last_of)
{
    stringbufferbase->test_find_last_of();
}

