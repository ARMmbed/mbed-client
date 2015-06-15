/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2minterfacefactory.h"

TEST_GROUP(M2MInterfaceFactory)
{
  Test_M2MInterfaceFactory* m2m_factory;

  void setup()
  {
    m2m_factory = new Test_M2MInterfaceFactory();
  }
  void teardown()
  {
    delete m2m_factory;
  }
};


TEST(M2MInterfaceFactory, create_interface)
{
    m2m_factory->test_create_interface();
}

TEST(M2MInterfaceFactory, create_device)
{
     m2m_factory->test_create_device();
}

TEST(M2MInterfaceFactory, create_security)
{
    m2m_factory->test_create_security();
}

TEST(M2MInterfaceFactory, create_server)
{
    m2m_factory->test_create_server();
}

TEST(M2MInterfaceFactory, create_object)
{
    m2m_factory->test_create_object();
}
