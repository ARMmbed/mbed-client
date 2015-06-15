/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_nsdlaccesshelper.h"

TEST_GROUP(NsdlAccessHelper)
{
  Test_NsdlAccessHelper* nsdl;

  void setup()
  {
    nsdl = new Test_NsdlAccessHelper();
  }
  void teardown()
  {
    delete nsdl;
  }
};


TEST(NsdlAccessHelper, create)
{
    CHECK(nsdl != NULL);
}

TEST(NsdlAccessHelper, test_nsdl_c_callback)
{
    nsdl->test_nsdl_c_callback();
}

TEST(NsdlAccessHelper, test_nsdl_c_memory_alloc)
{
    nsdl->test_nsdl_c_memory_alloc();
}

TEST(NsdlAccessHelper, test_nsdl_c_memory_free)
{
    nsdl->test_nsdl_c_memory_free();
}

TEST(NsdlAccessHelper, test_nsdl_c_send_to_server)
{
    nsdl->test_nsdl_c_send_to_server();
}

TEST(NsdlAccessHelper, test_nsdl_c_received_from_server)
{
    nsdl->test_nsdl_c_received_from_server();
}

TEST(NsdlAccessHelper, test_nsdl_c_bootstrap_done)
{
    nsdl->test_nsdl_c_bootstrap_done();
}

TEST(NsdlAccessHelper, test_socket_malloc)
{
    nsdl->test_socket_malloc();
}

TEST(NsdlAccessHelper, test_socket_free)
{
    nsdl->test_socket_free();
}

