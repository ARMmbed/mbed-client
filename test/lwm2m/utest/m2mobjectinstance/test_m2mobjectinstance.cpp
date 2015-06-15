/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mobjectinstance.h"
#include "m2mresource_stub.h"
#include "m2mbase_stub.h"

Test_M2MObjectInstance::Test_M2MObjectInstance()
{
    object = new M2MObjectInstance("name");
}

void Test_M2MObjectInstance::test_copy_constructor()
{
    M2MResource *res = new M2MResource("name","type",M2MBase::Static);
    object->_resource_list.push_back(res);

    M2MObjectInstance* copy = new M2MObjectInstance(*object);

    CHECK(1 == copy->_resource_list.size());
    delete copy;
}


Test_M2MObjectInstance::~Test_M2MObjectInstance()
{
    m2mresource_stub::clear();
    m2mbase_stub::clear();
    delete object;
}

void Test_M2MObjectInstance::test_create_static_resource()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;
    u_int8_t value[] = {"value"};

    m2mbase_stub::bool_value = true;
    M2MResource * res = object->create_static_resource("name","type",value,(u_int32_t)sizeof(value),false);

    CHECK(res != NULL);
    CHECK(1 == object->_resource_list.size());

    m2mbase_stub::bool_value = false;
    res = object->create_static_resource("name","type",value,(u_int32_t)sizeof(value));
    CHECK(res == NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_create_dynamic_resource()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;

    M2MResource * res = object->create_dynamic_resource("name","type",false,false);
    CHECK(res != NULL);
    CHECK(1 == object->_resource_list.size());

    M2MResource * res1 = object->create_dynamic_resource("name","type",false,false);
    CHECK(res1 != NULL);
    CHECK(2 == object->_resource_list.size());

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_remove_resource()
{
    CHECK(false == object->remove_resource("name", 0));

    M2MResource *res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;

    m2mresource_stub::bool_value = true;
    CHECK(true == object->remove_resource("name", 0));
    CHECK(0 == object->_resource_list.size());

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_resource()
{
    M2MResource *res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;

    M2MResource *result = object->resource("name", 0);
    CHECK(result != NULL);

    res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    m2mbase_stub::int_value = 1;

    result = object->resource("name", 1);
    CHECK(result != NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_resources()
{
    M2MResource *res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    M2MResourceList resources =object->resources();

    CHECK(2 == resources.size());
}

void Test_M2MObjectInstance::test_resource_count()
{
    M2MResource *res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;

    CHECK(2 == object->resource_count("name"));

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_total_resource_count()
{
    M2MResource *res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    res = new M2MResource("name1","type1",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    CHECK(2 == object->resource_count());
}

void Test_M2MObjectInstance::test_base_type()
{
    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(M2MBase::ObjectInstance == object->base_type());
}

void Test_M2MObjectInstance::test_handle_observation_attribute()
{
    char* c = {"value"};
    bool ret = object->handle_observation_attribute(c);
    CHECK(ret == false);

    M2MResource *res = new M2MResource("name","type",M2MBase::Static,true);
    object->_resource_list.push_back(res);

    m2mresource_stub::bool_value = true;
    ret = object->handle_observation_attribute(c);
    CHECK(ret == true);
}
