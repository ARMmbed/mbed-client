/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mdevice.h"

TEST_GROUP(M2MDevice)
{
  Test_M2MDevice* m2m_device;

  void setup()
  {
    m2m_device = new Test_M2MDevice();
  }
  void teardown()
  {
    delete m2m_device;
  }
};

TEST(M2MDevice, Create)
{
    CHECK(m2m_device != NULL);
}

TEST(M2MDevice, create_resource_string)
{
    m2m_device->test_create_resource_string();
}

TEST(M2MDevice, create_resource_int)
{
    m2m_device->test_create_resource_int();
}

TEST(M2MDevice, create_resource_no_param)
{
    m2m_device->test_create_resource_no_param();
}

TEST(M2MDevice, delete_resource)
{
    m2m_device->test_delete_resource();
}

TEST(M2MDevice, set_resource_value_int)
{
    m2m_device->test_set_resource_value_int();
}

TEST(M2MDevice, set_resource_value_string)
{
    m2m_device->test_set_resource_value_string();
}

TEST(M2MDevice, resource_value_int)
{
    m2m_device->test_resource_value_int();
}

TEST(M2MDevice, resource_value_string)
{
    m2m_device->test_resource_value_string();
}

TEST(M2MDevice, is_resource_present)
{
    m2m_device->test_is_resource_present();
}

TEST(M2MDevice, per_resource_count)
{
    m2m_device->test_per_resource_count();
}

TEST(M2MDevice, total_resource_count)
{
    m2m_device->test_total_resource_count();
}
