/*
 * Copyright (c) 2015 ARM. All rights reserved.
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
