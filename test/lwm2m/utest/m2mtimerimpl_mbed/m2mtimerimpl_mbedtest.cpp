/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerimpl_mbed.h"

TEST_GROUP(M2MTimerImpl_mbed)
{
  Test_M2MTimerImpl_mbed* m2m_timer;

  void setup()
  {
    m2m_timer = new Test_M2MTimerImpl_mbed();
  }
  void teardown()
  {
    delete m2m_timer;
  }
};

TEST(M2MTimerImpl_mbed, Create)
{
    CHECK(m2m_timer != NULL);
}

TEST(M2MTimerImpl_mbed, copy_constructor)
{
    m2m_timer->test_copy_constructor();
}

TEST(M2MTimerImpl_mbed, start_timer)
{
    m2m_timer->test_start_timer();
}

TEST(M2MTimerImpl_mbed, stop_timer)
{
    m2m_timer->test_stop_timer();
}

TEST(M2MTimerImpl_mbed, timer_expired)
{
    m2m_timer->test_timer_expired();
}
