/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_TLV_SERIALIZER_H
#define TEST_M2M_TLV_SERIALIZER_H

#include "m2mtlvserializer.h"

class Test_M2MTLVSerializer
{
public:    
    Test_M2MTLVSerializer();

    virtual ~Test_M2MTLVSerializer();

    void test_serialize_object();

    void test_serialize_object_instance();

    M2MTLVSerializer *serializer;
};

#endif // TEST_M2M_TLV_SERIALIZER_H
