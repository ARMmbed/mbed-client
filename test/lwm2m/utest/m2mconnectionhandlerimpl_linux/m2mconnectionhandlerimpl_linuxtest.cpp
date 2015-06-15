/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerimpl_linux.h"

TEST_GROUP(M2MConnectionHandlerImpl_linux)
{
  Test_M2MConnectionHandlerImpl_linux* handler;

  void setup()
  {
    handler = new Test_M2MConnectionHandlerImpl_linux();
  }
  void teardown()
  {
    delete handler;
  }
};


TEST(M2MConnectionHandlerImpl_linux, create)
{
    CHECK(handler != NULL);
}

TEST(M2MConnectionHandlerImpl_linux, test_bind_connection)
{
    handler->test_bind_connection();
}

TEST(M2MConnectionHandlerImpl_linux, test_resolve_server_address)
{
    handler->test_resolve_server_address();
}

TEST(M2MConnectionHandlerImpl_linux, test_listen_for_data)
{
    handler->test_listen_for_data();
}

TEST(M2MConnectionHandlerImpl_linux, test_data_receive)
{
    handler->test_data_receive();
}

TEST(M2MConnectionHandlerImpl_linux, test_send_data)
{
    handler->test_send_data();
}

TEST(M2MConnectionHandlerImpl_linux, test_close_connection)
{
    handler->test_close_connection();
}
