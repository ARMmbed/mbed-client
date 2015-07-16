/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerpimpl_linux.h"

TEST_GROUP(M2MTimerPimpl_linux)
{
  Test_M2MTimerPimpl_linux* m2m_timer;

  void setup()
  {
      m2m_timer = new Test_M2MTimerPimpl_linux();
  }
  void teardown()
  {
      delete m2m_timer;
  }
};

TEST(M2MTimerPimpl_linux, Create)
{
    CHECK(m2m_timer != NULL);
}

TEST(M2MTimerPimpl_linux, start_timer)
{
    m2m_timer->test_start_timer();
}

TEST(M2MTimerPimpl_linux, stop_timer)
{
    m2m_timer->test_stop_timer();
}

TEST(M2MTimerPimpl_linux, timer_expired)
{
    m2m_timer->test_timer_expired();
}

TEST(M2MTimerPimpl_linux, test_run)
{
    m2m_timer->test_run();
}

TEST(M2MTimerPimpl_linux, test_start_dtls_timer)
{
    m2m_timer->test_start_dtls_timer();
}

TEST(M2MTimerPimpl_linux, test_is_intermediate_interval_passed)
{
    m2m_timer->test_is_intermediate_interval_passed();
}

TEST(M2MTimerPimpl_linux, test_is_total_interval_passed)
{
    m2m_timer->test_is_total_interval_passed();
}
