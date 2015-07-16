/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mtimer.h"

TEST_GROUP(M2MTimer)
{
  Test_M2MTimer* m2m_timer;

  void setup()
  {
    m2m_timer = new Test_M2MTimer();
  }
  void teardown()
  {
    delete m2m_timer;
  }
};

TEST(M2MTimer, Create)
{
    CHECK(m2m_timer != NULL);
}

TEST(M2MTimer, start_timer)
{
    m2m_timer->test_start_timer();
}

TEST(M2MTimer, stop_timer)
{
    m2m_timer->test_stop_timer();
}

TEST(M2MTimer, test_start_dtls_timer)
{
    m2m_timer->test_start_dtls_timer();
}

TEST(M2MTimer, test_is_intermediate_interval_passed)
{
    m2m_timer->test_is_intermediate_interval_passed();
}

TEST(M2MTimer, test_is_total_interval_passed)
{
    m2m_timer->test_is_total_interval_passed();
}
