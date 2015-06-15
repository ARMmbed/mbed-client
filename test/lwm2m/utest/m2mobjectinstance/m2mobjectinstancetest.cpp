/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mobjectinstance.h"

TEST_GROUP(M2MObjectInstance)
{
  Test_M2MObjectInstance* m2m_object_instance;

  void setup()
  {
    m2m_object_instance = new Test_M2MObjectInstance();
  }
  void teardown()
  {    
    delete m2m_object_instance;
  }
};

TEST(M2MObjectInstance, Create)
{
    CHECK(m2m_object_instance != NULL);
}

TEST(M2MObjectInstance, copy_constructor)
{
    m2m_object_instance->test_copy_constructor();
}

TEST(M2MObjectInstance, create_static_resource)
{
    m2m_object_instance->test_create_static_resource();
}

TEST(M2MObjectInstance, create_dynamic_resource)
{
    m2m_object_instance->test_create_dynamic_resource();
}

TEST(M2MObjectInstance, remove_resource)
{
    m2m_object_instance->test_remove_resource();
}

TEST(M2MObjectInstance, resource)
{
    m2m_object_instance->test_resource();
}

TEST(M2MObjectInstance, resources)
{
    m2m_object_instance->test_resources();
}

TEST(M2MObjectInstance, resource_count)
{
    m2m_object_instance->test_resource_count();
}

TEST(M2MObjectInstance, total_resource_count)
{
    m2m_object_instance->test_total_resource_count();
}

TEST(M2MObjectInstance, base_type)
{
    m2m_object_instance->test_base_type();
}

TEST(M2MObjectInstance, handle_observation_attribute)
{
    m2m_object_instance->test_handle_observation_attribute();
}
