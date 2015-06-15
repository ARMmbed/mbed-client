/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerimpl_mbed.h"

TEST_GROUP(M2MConnectionHandlerImpl_mbed)
{
  Test_M2MConnectionHandlerImpl_mbed* handler;

  void setup()
  {
    handler = new Test_M2MConnectionHandlerImpl_mbed();
  }
  void teardown()
  {
    delete handler;
  }
};


TEST(M2MConnectionHandlerImpl_mbed, create)
{
    CHECK(handler != NULL);
}

TEST(M2MConnectionHandlerImpl_mbed, test_constructor)
{
    handler->test_constructor();
}

TEST(M2MConnectionHandlerImpl_mbed, test_bind_connection)
{
    handler->test_bind_connection();
}

TEST(M2MConnectionHandlerImpl_mbed, test_resolve_server_address)
{
    handler->test_resolve_server_address();
}

TEST(M2MConnectionHandlerImpl_mbed, test_send_data)
{
    handler->test_send_data();
}

TEST(M2MConnectionHandlerImpl_mbed, test_listen_for_data)
{
    handler->test_listen_for_data();
}

TEST(M2MConnectionHandlerImpl_mbed, test_send_handler)
{
    handler->test_send_handler();
}

TEST(M2MConnectionHandlerImpl_mbed, test_receive_handler)
{
    handler->test_receive_handler();
}

TEST(M2MConnectionHandlerImpl_mbed, test_dns_handler)
{
    handler->test_dns_handler();
}

TEST(M2MConnectionHandlerImpl_mbed, test_error_handler)
{
    handler->test_error_handler();
}

TEST(M2MConnectionHandlerImpl_mbed, test_get_network_stack)
{
    handler->test_get_network_stack();
}

TEST(M2MConnectionHandlerImpl_mbed, test_close_connection)
{
    handler->test_close_connection();
}
