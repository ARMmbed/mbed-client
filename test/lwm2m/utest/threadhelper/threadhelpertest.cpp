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
#include "test_threadhelper.h"

TEST_GROUP(ThreadHelper)
{
  Test_ThreadHelper* threadhelper;

  void setup()
  {
    threadhelper = new Test_ThreadHelper();
  }
  void teardown()
  {
    delete threadhelper;
  }
};

TEST(ThreadHelper, Create)
{
    CHECK(threadhelper != NULL);
}

TEST(ThreadHelper, start)
{
    threadhelper->test_start();
}

TEST(ThreadHelper, join)
{
    threadhelper->test_join();
}

TEST(ThreadHelper, cancel)
{
    threadhelper->test_cancel();
}

TEST(ThreadHelper, run)
{
    threadhelper->run();
}

