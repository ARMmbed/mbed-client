/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mbase.h"


TEST_GROUP(M2MBase)
{
  Test_M2MBase* m2m_base;

  void setup()
  {
    m2m_base = new Test_M2MBase();
  }
  void teardown()
  {
    delete m2m_base;
  }
};

TEST(M2MBase, Create)
{
    CHECK(m2m_base != NULL);
}

TEST(M2MBase, copy_constructor)
{
    m2m_base->test_copy_constructor();
}

TEST(M2MBase, test_assignment_operator)
{
    m2m_base->test_assignment_operator();
}

TEST(M2MBase, set_operation)
{
    m2m_base->test_set_operation();
}

TEST(M2MBase, test_set_base_type)
{
    m2m_base->test_set_base_type();
}

TEST(M2MBase, set_interface_description)
{
    m2m_base->test_set_interface_description();
}

TEST(M2MBase, set_resource_type)
{
    m2m_base->test_set_resource_type();
}

TEST(M2MBase, set_coap_content_type)
{
    m2m_base->test_set_coap_content_type();
}

TEST(M2MBase, set_instance_id)
{
    m2m_base->test_set_instance_id();
}

TEST(M2MBase, observation_number)
{
    m2m_base->test_observation_number();
}

TEST(M2MBase, mode)
{
    m2m_base->test_mode();
}

TEST(M2MBase, observation_token)
{
    m2m_base->test_get_observation_token();
}

TEST(M2MBase, is_observable)
{
    m2m_base->test_is_observable();
}

TEST(M2MBase, set_observation_token)
{
    m2m_base->test_set_observation_token();
}

TEST(M2MBase, set_under_observation)
{
    m2m_base->test_set_under_observation();
}

TEST(M2MBase, set_observable)
{
    m2m_base->test_set_observable();
}

TEST(M2MBase, operation)
{
    m2m_base->test_operation();
}

TEST(M2MBase, name)
{
    m2m_base->test_name();
}

TEST(M2MBase, instance_id)
{
    m2m_base->test_instance_id();
}

TEST(M2MBase, interface_description)
{
    m2m_base->test_interface_description();
}

TEST(M2MBase, resource_type)
{
    m2m_base->test_resource_type();
}

TEST(M2MBase, coap_content_type)
{
    m2m_base->test_coap_content_type();
}

TEST(M2MBase, test_base_type)
{
    m2m_base->test_base_type();
}

//TEST(M2MBase, set_value)
//{
//    m2m_base->test_set_value();
//}

TEST(M2MBase, test_set_observation_number)
{
    m2m_base->test_set_observation_number();
}

//TEST(M2MBase, get_value)
//{
//    m2m_base->test_get_value();
//}

TEST(M2MBase, test_handle_observation_attribute)
{
    m2m_base->test_handle_observation_attribute();
}

TEST(M2MBase, test_observation_to_be_sent)
{
    m2m_base->test_observation_to_be_sent();
}

TEST(M2MBase, test_remove_resource_from_coap)
{
    m2m_base->test_remove_resource_from_coap();
}

TEST(M2MBase, test_remove_object_from_coap)
{
    m2m_base->test_remove_object_from_coap();
}
