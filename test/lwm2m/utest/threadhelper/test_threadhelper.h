/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_THREAD_HELPER_H
#define TEST_THREAD_HELPER_H

#include "lwm2m-client-linux/threadhelper.h"

class Test_ThreadHelper : public ThreadHelper
{
public:
    Test_ThreadHelper();

    virtual ~Test_ThreadHelper();

    void test_start();

    void test_join();

    void test_cancel();

    void run();

};

#endif // TEST_THREAD_HELPER_H
