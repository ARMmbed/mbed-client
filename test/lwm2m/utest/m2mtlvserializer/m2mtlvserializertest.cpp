/*
 * Copyright (c) 2015 ARM. All rights reserved.
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

