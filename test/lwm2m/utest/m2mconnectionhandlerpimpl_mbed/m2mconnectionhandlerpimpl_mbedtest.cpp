/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerpimpl_mbed.h"

TEST_GROUP(M2MConnectionHandlerPimpl_mbed)
{
  Test_M2MConnectionHandlerPimpl_mbed* handler;

  void setup()
  {
    handler = new Test_M2MConnectionHandlerPimpl_mbed();
  }
  void teardown()
  {
    delete handler;
  }
};


TEST(M2MConnectionHandlerPimpl_mbed, create)
{
    CHECK(handler != NULL);
}

TEST(M2MConnectionHandlerPimpl_mbed, test_constructor)
{
    handler->test_constructor();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_bind_connection)
{
    handler->test_bind_connection();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_resolve_server_address)
{
    handler->test_resolve_server_address();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_send_data)
{
    handler->test_send_data();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_start_listening_for_data)
{
    handler->test_start_listening_for_data();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_send_handler)
{
    handler->test_send_handler();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_receive_handler)
{
    handler->test_receive_handler();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_receive_handshake_handler)
{
    handler->test_receive_handshake_handler();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_dns_handler)
{
    handler->test_dns_handler();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_error_handler)
{
    handler->test_error_handler();
}


TEST(M2MConnectionHandlerPimpl_mbed, test_stop_listening)
{
    handler->test_stop_listening();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_sendToSocket)
{
    handler->test_sendToSocket();
}

TEST(M2MConnectionHandlerPimpl_mbed, test_receiveFromSocket)
{
    handler->test_receiveFromSocket();
}
