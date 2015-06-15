/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2minterfaceimpl.h"

TEST_GROUP(M2MInterfaceImpl)
{
  Test_M2MInterfaceImpl* m2m_interface_impl;

  void setup()
  {
    m2m_interface_impl = new Test_M2MInterfaceImpl();
  }
  void teardown()
  {
    delete m2m_interface_impl;
  }
};


TEST(M2MInterfaceImpl, create)
{
    CHECK(m2m_interface_impl->impl != NULL);
}

TEST(M2MInterfaceImpl, bootstrap)
{
    m2m_interface_impl->test_bootstrap();
}

TEST(M2MInterfaceImpl, cancel_bootstrap)
{
    m2m_interface_impl->test_cancel_bootstrap();
}

TEST(M2MInterfaceImpl, register_object)
{
    m2m_interface_impl->test_register_object();
}

TEST(M2MInterfaceImpl, update_registration)
{
    m2m_interface_impl->test_update_registration();
}

TEST(M2MInterfaceImpl, unregister_object)
{
    m2m_interface_impl->test_unregister_object();
}

TEST(M2MInterfaceImpl, coap_message_ready)
{
    m2m_interface_impl->test_coap_message_ready();
}

TEST(M2MInterfaceImpl, client_registered)
{
    m2m_interface_impl->test_client_registered();
}

TEST(M2MInterfaceImpl, test_registration_updated)
{
    m2m_interface_impl->test_registration_updated();
}

TEST(M2MInterfaceImpl, registration_error)
{
    m2m_interface_impl->test_registration_error();
}

TEST(M2MInterfaceImpl, client_unregistered)
{
    m2m_interface_impl->test_client_unregistered();
}

TEST(M2MInterfaceImpl, bootstrap_done)
{
    m2m_interface_impl->test_bootstrap_done();
}

TEST(M2MInterfaceImpl, bootstrap_error)
{
    m2m_interface_impl->test_bootstrap_error();
}

TEST(M2MInterfaceImpl, coap_data_processed)
{
    m2m_interface_impl->test_coap_data_processed();
}

TEST(M2MInterfaceImpl, test_value_updated)
{
    m2m_interface_impl->test_value_updated();
}

TEST(M2MInterfaceImpl, data_available)
{
    m2m_interface_impl->test_data_available();
}

TEST(M2MInterfaceImpl, socket_error)
{
    m2m_interface_impl->test_socket_error();
}

TEST(M2MInterfaceImpl, address_ready)
{
    m2m_interface_impl->test_address_ready();
}

TEST(M2MInterfaceImpl, data_sent)
{
    m2m_interface_impl->test_data_sent();
}

