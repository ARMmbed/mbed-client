/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mobjectinstance.h"
#include "m2mresource_stub.h"
#include "common_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mbase_stub.h"

Test_M2MObjectInstance::Test_M2MObjectInstance()
{
    object = new M2MObjectInstance("name");
}

void Test_M2MObjectInstance::test_copy_constructor()
{
    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static);
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
    M2MResource * res = object->create_static_resource("name","type",M2MResourceInstance::STRING,value,(u_int32_t)sizeof(value),false);

    CHECK(res != NULL);
    CHECK(1 == object->_resource_list.size());

    res = object->create_static_resource("","type",M2MResourceInstance::STRING,value,(u_int32_t)sizeof(value));
    CHECK(res == NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_create_static_resource_instance()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;
    u_int8_t value[] = {"value"};

    m2mbase_stub::bool_value = true;
    m2mresource_stub::bool_value = true;
    M2MResourceInstance *ins = object->create_static_resource_instance("name","type",
                                                                M2MResourceInstance::STRING,
                                                                value,(u_int32_t)sizeof(value),
                                                                0);

    CHECK(ins != NULL);
    CHECK(1 == object->_resource_list.size());

    delete ins;

    ins = object->create_static_resource_instance("name","type",
                                                  M2MResourceInstance::STRING,
                                                  value,(u_int32_t)sizeof(value),
                                                  1);

    CHECK(ins != NULL);
    CHECK(1 == object->_resource_list.size());

    delete ins;

    ins = object->create_static_resource_instance("","type",
                                                  M2MResourceInstance::STRING,value,
                                                  (u_int32_t)sizeof(value),0);
    CHECK(ins == NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_create_dynamic_resource_instance()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;

    m2mresource_stub::bool_value = true;

    M2MResourceInstance * ins = object->create_dynamic_resource_instance("name","type",
                                                                         M2MResourceInstance::STRING,
                                                                         false,0);
    CHECK(ins != NULL);
    CHECK(1 == object->_resource_list.size());

    delete ins;

    ins = object->create_dynamic_resource_instance("name","type",
                                                   M2MResourceInstance::STRING,
                                                   false,1);
    CHECK(ins != NULL);
    CHECK(1 == object->_resource_list.size());

    delete ins;

    ins = object->create_dynamic_resource_instance("","type",
                                                   M2MResourceInstance::STRING,
                                                   false,1);

    CHECK(ins == NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_create_dynamic_resource()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;

    M2MResource * res = object->create_dynamic_resource("name","type",M2MResourceInstance::STRING,false,false);
    CHECK(res != NULL);
    CHECK(1 == object->_resource_list.size());

    M2MResource * res1 = object->create_dynamic_resource("name","type",M2MResourceInstance::STRING,false,false);
    CHECK(res1 != NULL);
    CHECK(2 == object->_resource_list.size());

    M2MResource * res2 = object->create_dynamic_resource("","type",M2MResourceInstance::STRING,false,false);
    CHECK(res2 == NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_remove_resource()
{
    CHECK(false == object->remove_resource("name"));

    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;
    m2mbase_stub::void_value = malloc(20);

    m2mresource_stub::bool_value = true;
    CHECK(true == object->remove_resource("name"));
    CHECK(0 == object->_resource_list.size());

    free(m2mbase_stub::void_value);
    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_remove_resource_instance()
{
    CHECK(false == object->remove_resource_instance("name",0));

    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;
    m2mbase_stub::void_value = malloc(20);

    m2mresource_stub::bool_value = true;

    M2MResourceInstance *ins = new M2MResourceInstance("name","type",M2MResourceInstance::STRING);

    m2mresource_stub::list.push_back(ins);

    CHECK(true == object->remove_resource_instance("name",0));
    CHECK(0 == object->_resource_list.size());

    free(m2mbase_stub::void_value);

    delete ins;
    ins = NULL;

    delete name;
    name = NULL;
}


void Test_M2MObjectInstance::test_resource()
{
    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;

    M2MResource *result = object->resource("name");
    CHECK(result != NULL);

    res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    m2mbase_stub::int_value = 1;

    result = object->resource("name");
    CHECK(result != NULL);

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_resources()
{
    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    M2MResourceList resources =object->resources();

    CHECK(2 == resources.size());
}

void Test_M2MObjectInstance::test_resource_count()
{
    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    String *name = new String("name");
    m2mbase_stub::string_value = name;
    m2mbase_stub::int_value = 0;

    m2mresource_stub::bool_value = true;
    m2mresource_stub::int_value = 1;

    CHECK(2 == object->resource_count("name"));

    m2mresource_stub::bool_value = false;

    CHECK(2 == object->resource_count("name"));

    delete name;
    name = NULL;
}

void Test_M2MObjectInstance::test_total_resource_count()
{
    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    res = new M2MResource("name1","type1",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    m2mresource_stub::bool_value = true;
    m2mresource_stub::int_value = 1;

    CHECK(2 == object->resource_count());

    m2mresource_stub::bool_value = false;

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

    M2MResource *res = new M2MResource("name","type",M2MResourceInstance::STRING,M2MBase::Static,true);
    object->_resource_list.push_back(res);

    m2mresource_stub::bool_value = true;
    ret = object->handle_observation_attribute(c);
    CHECK(ret == true);
}

void Test_M2MObjectInstance::test_handle_get_request()
{

}

void Test_M2MObjectInstance::test_handle_put_request()
{

}

void Test_M2MObjectInstance::test_handle_post_request()
{

}
