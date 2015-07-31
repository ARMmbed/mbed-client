/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mresourceinstance.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "m2mbase_stub.h"

class MyTest{
public:
    void execute_function(void */*argument*/) {
        visited = true;
    }

    bool visited;
};

Test_M2MResourceInstance::Test_M2MResourceInstance()
{
    resource_instance = new M2MResourceInstance("name",
                                                "resource_type",
                                                 M2MResourceInstance::STRING);
}

void Test_M2MResourceInstance::test_copy_constructor()
{
    u_int8_t value[] = {"value"};
    resource_instance->set_value(value,(u_int32_t)sizeof(value));

    M2MResourceInstance* copy = new M2MResourceInstance(*resource_instance);
    u_int8_t* out_value = (u_int8_t*)malloc(sizeof(u_int8_t));
    u_int32_t out_size;

    uint8_t* ptr = (uint8_t*)malloc((uint32_t)sizeof(value));
    m2mbase_stub::value = ptr;
    memset(m2mbase_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mbase_stub::value,value,sizeof(value));
    m2mbase_stub::int_value = (uint32_t)sizeof(value);

    copy->get_value(out_value,out_size);

    CHECK(out_size == sizeof(value));

    free(out_value);
    free(ptr);

    delete copy;
}

void Test_M2MResourceInstance::test_resource_instance_type()
{

}

Test_M2MResourceInstance::~Test_M2MResourceInstance()
{
    delete resource_instance;
}

void Test_M2MResourceInstance::test_base_type()
{
    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(M2MBase::Resource == resource_instance->base_type());
}

void Test_M2MResourceInstance::test_handle_observation_attribute()
{
    char *d = "s";
    CHECK(false == resource_instance->handle_observation_attribute(d));
}

void Test_M2MResourceInstance::test_set_execute_function()
{
    MyTest test;
    resource_instance->set_execute_function(execute_callback(&test,&MyTest::execute_function));
}

void Test_M2MResourceInstance::test_execute()
{
    MyTest test;
    void *args = NULL;
    resource_instance->set_execute_function(execute_callback(&test,&MyTest::execute_function));

    resource_instance->execute(args);
}

void Test_M2MResourceInstance::test_set_value()
{

}

void Test_M2MResourceInstance::test_get_value()
{

}

void Test_M2MResourceInstance::test_value()
{

}

void Test_M2MResourceInstance::test_value_length()
{

}

void Test_M2MResourceInstance::test_handle_get_request()
{

}

void Test_M2MResourceInstance::test_handle_put_request()
{

}

void Test_M2MResourceInstance::test_handle_post_request()
{

}
