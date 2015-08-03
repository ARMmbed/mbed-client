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
#include "test_m2mtlvdeserializer.h"


TEST_GROUP(M2MTLVDeserializer)
{
  Test_M2MTLVDeserializer* m2m_deserializer;

  void setup()
  {
    m2m_deserializer = new Test_M2MTLVDeserializer();
  }
  void teardown()
  {
    delete m2m_deserializer;
  }
};

TEST(M2MTLVDeserializer, Create)
{
    CHECK(m2m_deserializer != NULL);
}

TEST(M2MTLVDeserializer, is_object_instance)
{
    m2m_deserializer->test_is_object_instance();
}

TEST(M2MTLVDeserializer, is_resource)
{
    m2m_deserializer->test_is_resource();
}

TEST(M2MTLVDeserializer, is_multiple_resource)
{
    m2m_deserializer->test_is_multiple_resource();
}

TEST(M2MTLVDeserializer, is_resource_instance)
{
    m2m_deserializer->test_is_resource_instance();
}

TEST(M2MTLVDeserializer, deserialise_object_instances)
{
    m2m_deserializer->test_deserialise_object_instances();
}

TEST(M2MTLVDeserializer, deserialize_resources)
{
    m2m_deserializer->test_deserialize_resources();
}
