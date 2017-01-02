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
#include "test_m2mfirmware.h"
#include "m2mobject_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mbase_stub.h"
#include "nsdlaccesshelper_stub.h"

Test_M2MFirmware::Test_M2MFirmware()
{
    M2MObject* obj = new M2MObject("name", "name");
    m2mobject_stub::inst = new M2MObjectInstance(*obj,
                                                 "name","","");
    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");

    m2mobjectinstance_stub::create_resource_instance= new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              "name",
                                                                              "type",
                                                                              M2MResourceInstance::STRING,
                                                                              0,
                                                                              "name",
                                                                              false);
    m2mresource_stub::bool_value = true;
    m2mobjectinstance_stub::create_resource = new M2MResource(*m2mobject_stub::inst,
                                                              "name",
                                                              "type",
                                                              M2MResourceInstance::STRING,
                                                              false,
                                                              "name");
    firmware = M2MFirmware::get_instance();

    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    delete m2mobjectinstance_stub::create_resource_instance;
    m2mobjectinstance_stub::create_resource_instance = NULL;
    delete obj;
}

Test_M2MFirmware::~Test_M2MFirmware()
{
    delete m2mobject_stub::inst;
    m2mobject_stub::inst = NULL;

    M2MFirmware::delete_instance();
    firmware = NULL;
}

void Test_M2MFirmware::test_create_resource_string()
{
    CHECK(firmware->create_resource(M2MFirmware::Package, "test") == NULL);
    CHECK(firmware->create_resource(M2MFirmware::Update, "test") == NULL);
    CHECK(firmware->create_resource(M2MFirmware::State, "test") == NULL);

    m2mobjectinstance_stub::create_resource = new M2MResource(*m2mobject_stub::inst,
                                                              "name",
                                                              "type",
                                                              M2MResourceInstance::STRING,
                                                              false,
                                                              "name");
    CHECK(firmware->create_resource(M2MFirmware::PackageName,"test") != NULL);
    CHECK(M2MBase::GET_ALLOWED == m2mbase_stub::operation);
    CHECK(firmware->create_resource(M2MFirmware::PackageName,"") != NULL);

    // Check size limit
    String test = "";
    for (int i = 0; i <= 255; i++) {
        test.append("a", 1);
    }
    // 255 + null-termination
    CHECK(firmware->create_resource(M2MFirmware::PackageVersion,test) == NULL);

    test = "";
    for (int i = 0; i <= 254; i++) {
        test.append("a", 1);
    }
    // 254 + null-termination
    CHECK(firmware->create_resource(M2MFirmware::PackageVersion,test) != NULL);
    CHECK(M2MBase::GET_ALLOWED == m2mbase_stub::operation);


    CHECK(firmware->create_resource(M2MFirmware::Package,"test") == NULL);
    CHECK(firmware->create_resource(M2MFirmware::Update,"test") == NULL);
    CHECK(firmware->create_resource(M2MFirmware::State,"test") == NULL);
    CHECK(firmware->create_resource(M2MFirmware::UpdateSupportedObjects,"test") == NULL);
    CHECK(firmware->create_resource(M2MFirmware::UpdateResult,"test") == NULL);

    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;
}

void Test_M2MFirmware::test_create_resource_int()
{
    m2mobjectinstance_stub::create_resource = new M2MResource(*m2mobject_stub::inst,
                                                              "name",
                                                              "type",
                                                              M2MResourceInstance::STRING,
                                                              false,
                                                              "name");

    CHECK(firmware->create_resource(M2MFirmware::PackageVersion,10) == NULL);
    CHECK(firmware->create_resource(M2MFirmware::PackageName,10) == NULL);
    CHECK(firmware->create_resource(M2MFirmware::Package,10) == NULL);
    CHECK(firmware->create_resource(M2MFirmware::Update,10) == NULL);

    m2mbase_stub::void_value = malloc(20);

    CHECK(firmware->create_resource(M2MFirmware::UpdateSupportedObjects,false) != NULL);
    CHECK(firmware->create_resource(M2MFirmware::UpdateSupportedObjects,true) != NULL);
    CHECK(firmware->create_resource(M2MFirmware::UpdateSupportedObjects,5) == NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(firmware->create_resource(M2MFirmware::Update,10) == NULL);

    free(m2mbase_stub::void_value);
    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;
}


void Test_M2MFirmware::test_delete_resource()
{
    m2mobjectinstance_stub::bool_value = true;

    CHECK(firmware->delete_resource(M2MFirmware::PackageName) == true);
    CHECK(firmware->delete_resource(M2MFirmware::PackageVersion) == true);
    CHECK(firmware->delete_resource(M2MFirmware::UpdateSupportedObjects) == true);

    CHECK(firmware->delete_resource(M2MFirmware::Package) == false);
    CHECK(firmware->delete_resource(M2MFirmware::PackageUri) == false);
    CHECK(firmware->delete_resource(M2MFirmware::State) == false);
    CHECK(firmware->delete_resource(M2MFirmware::UpdateResult) == false);
    CHECK(firmware->delete_resource(M2MFirmware::Update) == false);
}

void Test_M2MFirmware::test_set_resource_value_string()
{
    m2mresourceinstance_stub::value = NULL;
    m2mbase_stub::bool_value = true;
    m2mresourceinstance_stub::bool_value = true;

    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");
    m2mresource_stub::bool_value = false;

    m2mbase_stub::void_value = malloc(20);

    CHECK(firmware->set_resource_value(M2MFirmware::PackageName,"test") == true);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageVersion,"test") == true);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageUri,"test") == true);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageUri,"") == true);

    // Check size limit
    String test = "";
    for (int i = 0; i <= 255; i++) {
        test.append("a", 1);
    }
    CHECK(firmware->set_resource_value(M2MFirmware::PackageUri,test) == false);

    CHECK(firmware->set_resource_value(M2MFirmware::Package,"test") == false);
    CHECK(firmware->set_resource_value(M2MFirmware::State,"test") == false);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateResult,"test") == false);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateSupportedObjects,"test") == false);
    CHECK(firmware->set_resource_value(M2MFirmware::Update,"test") == false);

    free(m2mbase_stub::void_value);
    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;
}

void Test_M2MFirmware::test_set_resource_value_int()
{
    m2mresourceinstance_stub::bool_value = true;

    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");
    m2mresource_stub::bool_value = false;

    m2mbase_stub::void_value = malloc(20);

    CHECK(firmware->set_resource_value(M2MFirmware::PackageName,10) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageVersion,10) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageUri,10) == false);

    CHECK(firmware->set_resource_value(M2MFirmware::Package,10) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::State,2) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::State,1) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::State,3) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::State,4) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::State,0) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateResult,2) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateResult,20) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateSupportedObjects,true) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::Update,10) == false);
    CHECK(firmware->check_value_range(M2MFirmware::Update,10) == false);

    free(m2mbase_stub::void_value);
    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;
}


void Test_M2MFirmware::test_resource_value_int()
{
    uint8_t value[] = {"10"};
    uint8_t* ptr = (uint8_t*)malloc((uint32_t)sizeof(value));
    m2mresourceinstance_stub::value = ptr;
    memset(m2mresourceinstance_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mresourceinstance_stub::value, value,sizeof(value));
    m2mresourceinstance_stub::int_value = (uint32_t)sizeof(value);

    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");

    m2mresourceinstance_stub::bool_value = true;
    m2mresource_stub::bool_value = false;
    m2mbase_stub::void_value = malloc(20);

    CHECK(firmware->resource_value_int(M2MFirmware::State) == 10);
    CHECK(firmware->resource_value_int(M2MFirmware::UpdateResult) == 10);
    CHECK(firmware->resource_value_int(M2MFirmware::UpdateSupportedObjects) == 10);
    CHECK(firmware->resource_value_int(M2MFirmware::PackageName) == -1);
    CHECK(firmware->resource_value_int(M2MFirmware::Package) == -1);
    CHECK(firmware->resource_value_int(M2MFirmware::PackageUri) == -1);
    CHECK(firmware->resource_value_int(M2MFirmware::PackageVersion) == -1);
    CHECK(firmware->resource_value_int(M2MFirmware::Update) == -1);

    free(m2mbase_stub::void_value);
    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    free(ptr);
}

void Test_M2MFirmware::test_resource_value_string()
{
    String test = "string";
    uint8_t value[] = {"string"};
    m2mresourceinstance_stub::value = (uint8_t*)malloc((uint32_t)sizeof(value));
    memset(m2mresourceinstance_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mresourceinstance_stub::value,value,sizeof(value));
    m2mresourceinstance_stub::int_value = (uint32_t)sizeof(value);

    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");

    m2mresourceinstance_stub::bool_value = true;
    m2mresource_stub::bool_value = false;
    m2mbase_stub::void_value = malloc(20);

    CHECK(firmware->resource_value_string(M2MFirmware::PackageUri) == test);
    CHECK(firmware->resource_value_string(M2MFirmware::PackageName) == test);
    CHECK(firmware->resource_value_string(M2MFirmware::PackageVersion) == test);
    CHECK(firmware->resource_value_string(M2MFirmware::Package) == "");
    CHECK(firmware->resource_value_string(M2MFirmware::Update) == "");
    CHECK(firmware->resource_value_string(M2MFirmware::UpdateResult) == "");
    CHECK(firmware->resource_value_string(M2MFirmware::UpdateSupportedObjects) == "");
    CHECK(firmware->resource_value_string(M2MFirmware::State) == "");

    free(m2mbase_stub::void_value);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    free(m2mresourceinstance_stub::value);
    m2mresourceinstance_stub::value = NULL;
}

void Test_M2MFirmware::test_is_resource_present()
{
    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");
    m2mresource_stub::bool_value = false;
    CHECK(firmware->is_resource_present(M2MFirmware::PackageUri) == true);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    CHECK(firmware->is_resource_present(M2MFirmware::PackageName) == false);
    CHECK(firmware->is_resource_present(M2MFirmware::PackageVersion) == false);
}

void Test_M2MFirmware::test_per_resource_count()
{
    m2mobjectinstance_stub::int_value = 1;
    CHECK(firmware->per_resource_count(M2MFirmware::Package) == 1);

    m2mobjectinstance_stub::int_value = 0;

    CHECK(firmware->per_resource_count(M2MFirmware::PackageVersion) == 0);
}



void Test_M2MFirmware::test_total_resource_count()
{
    M2MResource res(*m2mobject_stub::inst,
                    "name",
                    "type",
                    M2MResourceInstance::STRING,
                    false,
                    "name");

    m2mobjectinstance_stub::resource_list.push_back(&res);
    M2MResource res2(*m2mobject_stub::inst,
                     "name",
                     "type",
                     M2MResourceInstance::STRING,
                     false,
                     "name");

    m2mobjectinstance_stub::resource_list.push_back(&res2);

    CHECK(firmware->total_resource_count() == 2);

    m2mobjectinstance_stub::resource_list.clear();
}

void Test_M2MFirmware::test_resource_value_buffer()
{
    m2mresourceinstance_stub::bool_value = true;

    uint8_t value[] = {"buffer"};
    uint8_t *out_value = NULL;

    m2mresourceinstance_stub::value = (uint8_t*)malloc((uint32_t)sizeof(value));
    memset(m2mresourceinstance_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mresourceinstance_stub::value,value,sizeof(value));
    m2mresourceinstance_stub::int_value = (uint16_t)sizeof(value);

    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");

    CHECK(firmware->resource_value_buffer(M2MFirmware::Package,out_value) != 0);

    CHECK(firmware->resource_value_buffer(M2MFirmware::PackageName,out_value) == 0);
    CHECK(firmware->resource_value_buffer(M2MFirmware::PackageUri,out_value) == 0);
    CHECK(firmware->resource_value_buffer(M2MFirmware::PackageVersion,out_value) == 0);
    CHECK(firmware->resource_value_buffer(M2MFirmware::Update,out_value) == 0);
    CHECK(firmware->resource_value_buffer(M2MFirmware::State,out_value) == 0);
    CHECK(firmware->resource_value_buffer(M2MFirmware::UpdateResult,out_value) == 0);
    CHECK(firmware->resource_value_buffer(M2MFirmware::UpdateSupportedObjects,out_value) == 0);


    free(out_value);
    out_value = NULL;

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    free(m2mresourceinstance_stub::value);
    m2mresourceinstance_stub::value = NULL;
}

void Test_M2MFirmware::test_set_resource_value_buffer()
{
    m2mresourceinstance_stub::bool_value = true;

    uint8_t value[] = {"value"};
    uint16_t length = (uint16_t)sizeof(value);

    m2mobjectinstance_stub::resource = new M2MResource(*m2mobject_stub::inst,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");

    CHECK(firmware->set_resource_value(M2MFirmware::Package,value,length) == true);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageName,value,length) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageUri,value,length) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::PackageVersion,value,length) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::State,value,length) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::Update,value,length) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateResult,value,length) == false);
    CHECK(firmware->set_resource_value(M2MFirmware::UpdateSupportedObjects,value,length) == false);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;
}

void Test_M2MFirmware::test_resource_name()
{
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::Package), "0");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::PackageUri), "1");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::Update), "2");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::State), "3");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::UpdateSupportedObjects), "4");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::UpdateResult), "5");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::PackageName), "6");
    STRCMP_EQUAL(firmware->resource_name(M2MFirmware::PackageVersion), "7");
}
