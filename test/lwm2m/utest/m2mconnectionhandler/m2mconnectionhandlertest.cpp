/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandler.h"

TEST_GROUP(M2MConnectionHandler)
{
  Test_M2MConnectionHandler* m2m_connection_handler;

  void setup()
  {
    m2m_connection_handler = new Test_M2MConnectionHandler();
  }
  void teardown()
  {
    delete m2m_connection_handler;
  }
};


TEST(M2MConnectionHandler, create)
{
    CHECK(m2m_connection_handler != NULL);
}

TEST(M2MConnectionHandler, bind_connection)
{
    m2m_connection_handler->test_bind_connection();
}

TEST(M2MConnectionHandler, resolve_server_address)
{
    m2m_connection_handler->test_resolve_server_address();
}

TEST(M2MConnectionHandler, send_data)
{
    m2m_connection_handler->test_send_data();
}

TEST(M2MConnectionHandler, listen_for_data)
{
    m2m_connection_handler->test_listen_for_data();
}

TEST(M2MConnectionHandler, close_connection)
{
    m2m_connection_handler->test_close_connection();
}
