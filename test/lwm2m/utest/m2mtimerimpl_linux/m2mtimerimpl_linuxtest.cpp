/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mtimerimpl_linux.h"

TEST_GROUP(M2MTimerImpl_linux)
{
  Test_M2MTimerImpl_linux* m2m_timer;

  void setup()
  {
    m2m_timer = new Test_M2MTimerImpl_linux();
  }
  void teardown()
  {
    delete m2m_timer;
  }
};

TEST(M2MTimerImpl_linux, Create)
{
    CHECK(m2m_timer != NULL);
}

TEST(M2MTimerImpl_linux, copy_constructor)
{
    m2m_timer->test_copy_constructor();
}

TEST(M2MTimerImpl_linux, start_timer)
{
    m2m_timer->test_start_timer();
}

TEST(M2MTimerImpl_linux, stop_timer)
{
    m2m_timer->test_stop_timer();
}

TEST(M2MTimerImpl_linux, timer_expired)
{
    m2m_timer->test_timer_expired();
}

TEST(M2MTimerImpl_linux, test_thread_function)
{
    m2m_timer->test_thread_function();
}

