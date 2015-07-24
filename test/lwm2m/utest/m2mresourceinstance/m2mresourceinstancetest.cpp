/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mresourceinstance.h"

TEST_GROUP(M2MResourceInstance)
{
  Test_M2MResourceInstance* m2m_resourceinstance;

  void setup()
  {
    m2m_resourceinstance = new Test_M2MResourceInstance();
  }
  void teardown()
  {
    delete m2m_resourceinstance;
  }
};

TEST(M2MResourceInstance, Create)
{
    CHECK(m2m_resourceinstance != NULL);
}

TEST(M2MResourceInstance, copy_constructor)
{
    m2m_resourceinstance->test_copy_constructor();
}

TEST(M2MResourceInstance, base_type)
{
    m2m_resourceinstance->test_base_type();
}

TEST(M2MResourceInstance, test_handle_observation_attribute)
{
    m2m_resourceinstance->test_handle_observation_attribute();
}

TEST(M2MResourceInstance, test_set_execute_function)
{
    m2m_resourceinstance->test_set_execute_function();
}

TEST(M2MResourceInstance, test_execute)
{
    m2m_resourceinstance->test_execute();
}

TEST(M2MResourceInstance, test_resource_instance_type)
{
    m2m_resourceinstance->test_resource_instance_type();
}

TEST(M2MResourceInstance, test_set_value)
{
    m2m_resourceinstance->test_set_value();
}

TEST(M2MResourceInstance, test_get_value)
{
    m2m_resourceinstance->test_get_value();
}

TEST(M2MResourceInstance, test_value)
{
    m2m_resourceinstance->test_value();
}

TEST(M2MResourceInstance, test_value_length)
{
    m2m_resourceinstance->test_value_length();
}

TEST(M2MResourceInstance, test_handle_get_request)
{
    m2m_resourceinstance->test_handle_get_request();
}

TEST(M2MResourceInstance, test_handle_put_request)
{
    m2m_resourceinstance->test_handle_put_request();
}

TEST(M2MResourceInstance, test_handle_post_request)
{
    m2m_resourceinstance->test_handle_post_request();
}
