/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"
int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}

//#include "../m2mbase/m2mbasetest.h"
#include "../m2mobject/m2mobjecttest.h"
