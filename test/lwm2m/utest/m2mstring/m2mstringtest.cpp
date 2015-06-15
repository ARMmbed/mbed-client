/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_m2mstring.h"

TEST_GROUP(M2MString)
{
  Test_M2MString* m2m_string;

  void setup()
  {
    m2m_string = new Test_M2MString();
  }
  void teardown()
  {
    delete m2m_string;
  }
};

TEST(M2MString, Create)
{
    CHECK(m2m_string != NULL);
}

TEST(M2MString, copy_constructor)
{
    m2m_string->test_copy_constructor();
}

TEST(M2MString, test_operator_assign)
{
    m2m_string->test_operator_assign();
}

TEST(M2MString, test_operator_add)
{
    m2m_string->test_operator_add();
}

TEST(M2MString, test_push_back)
{
    m2m_string->test_push_back();
}

TEST(M2MString, test_operator_equals)
{
    m2m_string->test_operator_equals();
}

TEST(M2MString, test_clear)
{
    m2m_string->test_clear();
}

TEST(M2MString, test_size)
{
    m2m_string->test_size();
}

TEST(M2MString, test_length)
{
    m2m_string->test_length();
}

TEST(M2MString, test_capacity)
{
    m2m_string->test_capacity();
}

TEST(M2MString, test_empty)
{
    m2m_string->test_empty();
}

TEST(M2MString, test_c_str)
{
    m2m_string->test_c_str();
}

TEST(M2MString, test_reserve)
{
    m2m_string->test_reserve();
}

TEST(M2MString, test_resize)
{
    m2m_string->test_resize();
}

TEST(M2MString, test_swap)
{
    m2m_string->test_swap();
}

TEST(M2MString, test_substr)
{
    m2m_string->test_substr();
}

TEST(M2MString, test_operator_get)
{
    m2m_string->test_operator_get();
}

TEST(M2MString, test_at)
{
    m2m_string->test_at();
}

TEST(M2MString, test_erase)
{
    m2m_string->test_erase();
}

TEST(M2MString, test_append)
{
    m2m_string->test_append();
}

TEST(M2MString, test_compare)
{
    m2m_string->test_compare();
}

TEST(M2MString, test_find_last_of)
{
    m2m_string->test_find_last_of();
}

TEST(M2MString, test_operator_lt)
{
    m2m_string->test_operator_lt();
}
