/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerpimpl_mbed.h"

TEST_GROUP(M2MTimerPimpl_mbed)
{
  Test_M2MTimerPimpl_mbed* m2m_timer;

  void setup()
  {
    m2m_timer = new Test_M2MTimerPimpl_mbed();
  }
  void teardown()
  {
    delete m2m_timer;
  }
};

TEST(M2MTimerPimpl_mbed, Create)
{
    CHECK(m2m_timer != NULL);
}

TEST(M2MTimerPimpl_mbed, start_timer)
{
    m2m_timer->test_start_timer();
}

TEST(M2MTimerPimpl_mbed, stop_timer)
{
    m2m_timer->test_stop_timer();
}

TEST(M2MTimerPimpl_mbed, timer_expired)
{
    m2m_timer->test_timer_expired();
}

TEST(M2MTimerPimpl_mbed, test_start_dtls_timer)
{
    m2m_timer->test_start_dtls_timer();
}

TEST(M2MTimerPimpl_mbed, test_dtls_timer_expired)
{
    m2m_timer->test_dtls_timer_expired();
}

TEST(M2MTimerPimpl_mbed, test_is_intermediate_interval_passed)
{
    m2m_timer->test_is_intermediate_interval_passed();
}

TEST(M2MTimerPimpl_mbed, test_is_total_interval_passed)
{
    m2m_timer->test_is_total_interval_passed();
}
