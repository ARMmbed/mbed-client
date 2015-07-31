/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mobject.h"

TEST_GROUP(M2MObject)
{
  Test_M2MObject* m2m_object;

  void setup()
  {
    m2m_object = new Test_M2MObject();
  }
  void teardown()
  {
    delete m2m_object;
  }
};

TEST(M2MObject, Create)
{
    CHECK(m2m_object != NULL);
}

TEST(M2MObject, base_type)
{
    m2m_object->test_base_type();
}

TEST(M2MObject, copy_constructor)
{
    m2m_object->test_copy_constructor();
}

TEST(M2MObject, create_object_instance)
{
    m2m_object->test_create_object_instance();
}

TEST(M2MObject, remove_object_instance)
{
    m2m_object->test_remove_object_instance();
}

TEST(M2MObject, object_instance)
{
    m2m_object->test_object_instance();
}

TEST(M2MObject, instance_count)
{
    m2m_object->test_instance_count();
}

TEST(M2MObject, instances)
{
    m2m_object->test_instances();
}

TEST(M2MObject, handle_observation_attribute)
{
    m2m_object->test_handle_observation_attribute();
}

TEST(M2MObject, handle_get_request)
{
    m2m_object->test_handle_get_request();
}

TEST(M2MObject, handle_put_request)
{
    m2m_object->test_handle_put_request();
}

TEST(M2MObject, handle_post_request)
{
    m2m_object->test_handle_post_request();
}
