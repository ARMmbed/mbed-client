/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_threadhelper.h"
#include "common_stub.h"

class Helper : ThreadHelper {

public:
    Helper(){}
    ~Helper(){}
    void run() {
        run_value = true;
    }
    bool run_value;
};

Test_ThreadHelper::Test_ThreadHelper()
{
}

Test_ThreadHelper::~Test_ThreadHelper()
{
    common_stub::clear();    
}

void Test_ThreadHelper::test_start()
{
    common_stub::int_value = 0;
    CHECK(start() == true);

    common_stub::int_value = 1;
    CHECK(start() == false);
}

void Test_ThreadHelper::test_join()
{
    common_stub::int_value = 0;
    CHECK(join() == true);

    common_stub::int_value = 1;
    CHECK(join() == false);
}

void Test_ThreadHelper::test_cancel()
{
    _thread_id = 0;
    CHECK(cancel() == true);

    _thread_id = 1;
    common_stub::int_value = 1;
    CHECK(cancel() == false);

    common_stub::int_value = 0;
    CHECK(cancel() == true);
}

void Test_ThreadHelper::run()
{
    // Test to check NULL safe code
    thread_function(NULL);

    Helper *helper = new Helper();
    thread_function(helper);

    CHECK(helper->run_value == true);

    delete helper;
}
