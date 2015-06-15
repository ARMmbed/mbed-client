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

TEST(M2MTimer, copy_constructor)
{
    m2m_timer->test_copy_constructor();
}

TEST(M2MTimer, start_timer)
{
    m2m_timer->test_start_timer();
}

TEST(M2MTimer, stop_timer)
{
    m2m_timer->test_stop_timer();
}

TEST(M2MTimer, timer_expired)
{
    m2m_timer->test_timer_expired();
}
