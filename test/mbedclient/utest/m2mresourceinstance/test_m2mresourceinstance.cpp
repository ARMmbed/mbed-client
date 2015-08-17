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
#include "test_m2mresourceinstance.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "m2mbase_stub.h"
#include "m2mobservationhandler.h"
#include "m2mreporthandler.h"

class MyTest{
public:
    void execute_function(void */*argument*/) {
        visited = true;
    }

    bool visited;
};

class TestReportObserver :  public M2MReportObserver{
public :
    TestReportObserver() {}
    ~TestReportObserver() {}
    void observation_to_be_sent(){ }
};

class  TestObservationHandler : M2MObservationHandler {
public :
    TestObservationHandler() {}
    ~TestObservationHandler() {}

    void observation_to_be_sent(M2MBase *){}

    void resource_to_be_deleted(const String &){}

    void value_updated(M2MBase *){}

    void remove_object(M2MBase *){}

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

    copy->get_value(out_value,out_size);
    CHECK(out_size == sizeof(value));
    free(out_value);

    delete copy;
}

void Test_M2MResourceInstance::test_assignment_constructor()
{
    u_int8_t value[] = {"value"};
    resource_instance->set_value(value,(u_int32_t)sizeof(value));

    M2MResourceInstance* res = new M2MResourceInstance("name",
                                                        "resource_type",
                                                         M2MResourceInstance::STRING);

    u_int8_t value1[] = {"value1"};
    res->set_value(value1,(u_int32_t)sizeof(value1));

    *res = *resource_instance;

    u_int8_t* out_value = (u_int8_t*)malloc(sizeof(u_int8_t));
    u_int32_t out_size;

    res->get_value(out_value,out_size);

    CHECK(out_size == sizeof(value));
    free(out_value);

    delete res;
}


void Test_M2MResourceInstance::test_resource_instance_type()
{
    CHECK(resource_instance->resource_instance_type() == M2MResourceInstance::STRING);
}

Test_M2MResourceInstance::~Test_M2MResourceInstance()
{
    delete resource_instance;
}

void Test_M2MResourceInstance::test_static_resource_instance()
{
    u_int8_t value[] = {"value"};
    M2MResourceInstance *res = new M2MResourceInstance("name1", "type1",
                                                       M2MResourceInstance::INTEGER,
                                                       value, (uint32_t)sizeof(value));

    CHECK(res != NULL);
    delete res;
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

    resource_instance->_resource_type = M2MResourceInstance::INTEGER;
    CHECK(false == resource_instance->handle_observation_attribute(d));

    resource_instance->_resource_type = M2MResourceInstance::FLOAT;
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
    u_int8_t value[] = {"value2"};
    resource_instance->_value = (u_int8_t*)malloc(sizeof(u_int8_t));

    CHECK(resource_instance->set_value(value,(u_int32_t)sizeof(value)) == true);
    CHECK( resource_instance->_value_length == sizeof(value));
    CHECK( *resource_instance->_value == *value);

    TestObservationHandler *observer = new TestObservationHandler;
    m2mbase_stub::observe = (M2MObservationHandler*)observer;

    u_int8_t value2[] = {"12"};
    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);

    delete m2mbase_stub::observe;
    m2mbase_stub::observe = NULL;

    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);

    resource_instance->_resource_type = M2MResourceInstance::INTEGER;

    m2mbase_stub::mode_value = M2MBase::Dynamic;

    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);

    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
}

void Test_M2MResourceInstance::test_get_value()
{
    u_int8_t test_value[] = {"value3"};
    u_int32_t value_length((u_int32_t)sizeof(test_value));

    u_int8_t* out_value = (u_int8_t *)malloc(1);
    u_int32_t out_size = 1;

    resource_instance->_value = (u_int8_t *)malloc(value_length);
    resource_instance->_value_length = value_length;
    memcpy((u_int8_t *)resource_instance->_value, (u_int8_t *)test_value, value_length);

    resource_instance->get_value(out_value,out_size);

    CHECK(out_size == value_length);

    free(out_value);
}

void Test_M2MResourceInstance::test_value()
{
    CHECK(resource_instance->value() == NULL);
}

void Test_M2MResourceInstance::test_value_length()
{
    CHECK(resource_instance->value_length() == 0);
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
