/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_threadhelper.h"

TEST_GROUP(ThreadHelper)
{
  Test_ThreadHelper* threadhelper;

  void setup()
  {
    threadhelper = new Test_ThreadHelper();
  }
  void teardown()
  {
    delete threadhelper;
  }
};

TEST(ThreadHelper, Create)
{
    CHECK(threadhelper != NULL);
}

TEST(ThreadHelper, start)
{
    threadhelper->test_start();
}

TEST(ThreadHelper, join)
{
    threadhelper->test_join();
}

TEST(ThreadHelper, cancel)
{
    threadhelper->test_cancel();
}

TEST(ThreadHelper, run)
{
    threadhelper->run();
}

