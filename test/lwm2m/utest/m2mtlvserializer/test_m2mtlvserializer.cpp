/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mtlvserializer.h"


Test_M2MTLVSerializer::Test_M2MTLVSerializer()
{
    serializer = new M2MTLVSerializer();
}

Test_M2MTLVSerializer::~Test_M2MTLVSerializer()
{
    delete serializer;
}

void Test_M2MTLVSerializer::test_serialize_object()
{

}

void Test_M2MTLVSerializer::test_serialize_object_instance()
{

}
