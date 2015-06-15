/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mreporthandler.h"


TEST_GROUP(M2MReportHandler)
{
  Test_M2MReportHandler* m2m_reporthandler;

  void setup()
  {
    m2m_reporthandler = new Test_M2MReportHandler();
  }
  void teardown()
  {
    delete m2m_reporthandler;
  }
};

TEST(M2MReportHandler, Create)
{
    CHECK(m2m_reporthandler != NULL);
}

TEST(M2MReportHandler, test_set_under_observation)
{
    m2m_reporthandler->test_set_under_observation();
}

TEST(M2MReportHandler, test_parse_notification_attribute)
{
    m2m_reporthandler->test_parse_notification_attribute();
}

TEST(M2MReportHandler, test_timer_expired)
{
    m2m_reporthandler->test_timer_expired();
}

TEST(M2MReportHandler, test_set_value)
{
    m2m_reporthandler->test_set_value();
}
