/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionsecurityimpl_mbedtls.h"


TEST_GROUP(M2MConnectionSecurityImpl_mbedtls)
{
  Test_M2MConnectionSecurityImpl* inst;

  void setup()
  {
    inst = new Test_M2MConnectionSecurityImpl();
  }
  void teardown()
  {
    delete inst;
  }
};

TEST(M2MConnectionSecurityImpl_mbedtls, Create)
{
    CHECK(inst != NULL);
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_constructor)
{
    inst->test_constructor();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_destructor)
{
    inst->test_destructor();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_reset)
{
    inst->test_reset();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_init)
{
    inst->test_init();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_connect)
{
    inst->test_connect();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_start_connecting_non_blocking)
{
    inst->test_start_connecting_non_blocking();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_continue_connecting)
{
    inst->test_continue_connecting();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_send_message)
{
    inst->test_send_message();
}

TEST(M2MConnectionSecurityImpl_mbedtls, test_read)
{
    inst->test_read();
}

