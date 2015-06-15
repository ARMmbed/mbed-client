/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mresource.h"

TEST_GROUP(M2MResource)
{
  Test_M2MResource* m2m_resource;

  void setup()
  {
    m2m_resource = new Test_M2MResource();
  }
  void teardown()
  {
    delete m2m_resource;
  }
};

TEST(M2MResource, Create)
{
    CHECK(m2m_resource != NULL);
}

TEST(M2MResource, copy_constructor)
{
    m2m_resource->test_copy_constructor();
}

TEST(M2MResource, base_type)
{
    m2m_resource->test_base_type();
}

TEST(M2MResource, muliptle_instances)
{
    m2m_resource->test_muliptle_instances();
}

TEST(M2MResource, test_handle_observation_attribute)
{
    m2m_resource->test_handle_observation_attribute();
}

TEST(M2MResource, test_set_execute_function)
{
    m2m_resource->test_set_execute_function();
}

TEST(M2MResource, test_execute)
{
    m2m_resource->test_execute();
}
