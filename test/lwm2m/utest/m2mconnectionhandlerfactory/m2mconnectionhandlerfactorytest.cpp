/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerfactory.h"

TEST_GROUP(M2MConnectionHandlerFactory)
{
  Test_M2MConnectionHandlerFactory* m2m_connection_handler;

  void setup()
  {
    m2m_connection_handler = new Test_M2MConnectionHandlerFactory();
  }
  void teardown()
  {
    delete m2m_connection_handler;
  }
};


TEST(M2MConnectionHandlerFactory, create)
{
    CHECK(m2m_connection_handler != NULL);
}

TEST(M2MConnectionHandlerFactory, test_createConnectionHandler)
{
    m2m_connection_handler->test_createConnectionHandler();
}
