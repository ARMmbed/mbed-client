/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TLV_DESERIALIZER_H
#define TEST_M2M_TLV_DESERIALIZER_H

#include "m2mtlvdeserializer.h"

class Test_M2MTLVDeserializer
{
public:
    Test_M2MTLVDeserializer();

    virtual ~Test_M2MTLVDeserializer();

    void test_is_object_instance();

    void test_is_resource();

    void test_is_multiple_resource();

    void test_is_resource_instance();

    void test_deserialise_object_instances();

    void test_deserialize_resources();

    M2MTLVDeserializer *deserializer;
};

#endif // TEST_M2M_TLV_DESERIALIZER_H
