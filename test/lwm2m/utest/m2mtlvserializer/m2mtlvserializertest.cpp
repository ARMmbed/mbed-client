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
#include "test_m2mtlvserializer.h"


TEST_GROUP(M2MTLVSerializer)
{
  Test_M2MTLVSerializer* m2m_serializer;

  void setup()
  {
    m2m_serializer = new Test_M2MTLVSerializer();
  }
  void teardown()
  {
    delete m2m_serializer;
  }
};

TEST(M2MTLVSerializer, Create)
{
    CHECK(m2m_serializer != NULL);
}

TEST(M2MTLVSerializer, serialize_object)
{
    m2m_serializer->test_serialize_object();
}

TEST(M2MTLVSerializer, serialize_object_instance)
{
    m2m_serializer->test_serialize_object_instance();
}

