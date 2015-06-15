/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_STRING_H
#define TEST_M2M_STRING_H

#include "m2mstring.h"

using namespace m2m;

class Test_M2MString
{
public:
    Test_M2MString();
    virtual ~Test_M2MString();

    void test_copy_constructor();
    void test_operator_assign();
    void test_operator_add();
    void test_push_back();
    void test_operator_equals();
    void test_clear();
    void test_size();
    void test_length();
    void test_capacity();
    void test_empty();
    void test_c_str();
    void test_reserve();
    void test_resize();
    void test_swap();
    void test_substr();
    void test_operator_get();
    void test_at();
    void test_erase();
    void test_append();
    void test_compare();
    void test_find_last_of();

    void test_operator_lt();

    String* str;
};

#endif // TEST_M2M_STRING_H
