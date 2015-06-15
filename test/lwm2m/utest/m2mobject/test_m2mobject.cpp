/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mobject.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mbase_stub.h"

Test_M2MObject::Test_M2MObject()
{
    object = new M2MObject("name");
}

Test_M2MObject::~Test_M2MObject()
{
    m2mobjectinstance_stub::clear();
    m2mresource_stub::clear();
    m2mbase_stub::clear();
    delete object;
}

void Test_M2MObject::test_copy_constructor()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;

    M2MObjectInstance *ins = new M2MObjectInstance("name");
    object->set_instance_id(0);
    object->_instance_list.push_back(ins);
    object->_instance_index = 0;

    M2MObject* copy = new M2MObject(*object);

    CHECK(1 == copy->_instance_list.size());

    delete copy;
    delete name;
    name = NULL;
}

void Test_M2MObject::test_create_object_instance()
{
    CHECK(object->create_object_instance() != NULL);
}

void Test_M2MObject::test_remove_object_instance()
{
    M2MObjectInstance *ins = new M2MObjectInstance("name");
    object->set_instance_id(0);
    object->_instance_list.push_back(ins);
    object->_instance_index = 0;

    CHECK(true == object->remove_object_instance(0));

    CHECK(false == object->remove_object_instance(0));
}

void Test_M2MObject::test_object_instance()
{
    String *test = new String("name");
    M2MObjectInstance *ins = new M2MObjectInstance(*test);
    object->set_instance_id(0);
    object->_instance_list.push_back(ins);
    object->_instance_index = 0;

    m2mbase_stub::string_value = test;

    M2MObjectInstance *obj = object->object_instance(0);

    CHECK(obj != NULL);
    CHECK(0 == obj->name().compare(0,test->size(),*test));

    delete test;
    test = NULL;
}

void Test_M2MObject::test_instances()
{
    String *test = new String("name");
    M2MObjectInstance *ins = new M2MObjectInstance(*test);
    ins->set_instance_id(0);
    object->_instance_list.push_back(ins);
    object->_instance_index = 0;

    M2MObjectInstance *ins1 = new M2MObjectInstance(*test);
    ins1->set_instance_id(1);
    object->_instance_list.push_back(ins1);
    object->_instance_index = 1;

    m2mbase_stub::string_value = test;

    M2MObjectInstanceList list = object->instances();

    M2MObjectInstance *obj = list[0];
    CHECK(2 == list.size());
    CHECK(0 == obj->name().compare(0,test->size(),*test));

    delete test;
    test = NULL;
}

void Test_M2MObject::test_instance_count()
{
    String test = "name";
    M2MObjectInstance *ins = new M2MObjectInstance(test);
    object->set_instance_id(0);
    object->_instance_list.push_back(ins);
    object->_instance_index = 0;

    ins = new M2MObjectInstance(test);
    object->set_instance_id(1);
    object->_instance_list.push_back(ins);
    object->_instance_index = 1;

    CHECK(2 == object->instance_count());
}

void Test_M2MObject::test_base_type()
{
    CHECK(M2MBase::Object == object->base_type());
}

void Test_M2MObject::test_handle_observation_attribute()
{
    char* c = {"value"};
    bool ret = object->handle_observation_attribute(c);
    CHECK(ret == false);

    M2MObjectInstance *ins = new M2MObjectInstance("name");
    object->set_instance_id(0);
    object->_instance_list.push_back(ins);
    object->_instance_index = 0;

    m2mobjectinstance_stub::bool_value = true;
    ret = object->handle_observation_attribute(c);
    CHECK(ret == true);
}
