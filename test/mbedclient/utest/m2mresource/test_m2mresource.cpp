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
#include "test_m2mresource.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "m2mbase_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mobjectinstance_stub.h"

class MyTest{
public:
    void execute_function(void */*argument*/) {
        visited = true;
    }

    bool visited;
};

class Callback : public M2MObjectInstanceCallback {

public:

    Callback(){}
    ~Callback(){}
    void notification_update(M2MBase::Observation) {
        visited = true;
    }

    void clear() {visited = false;}
    bool visited;
};

Test_M2MResource::Test_M2MResource()
{
    callback = new Callback();
    resource = new M2MResource(*callback,
                               "name",
                               "resource_type",
                               M2MResourceInstance::INTEGER,
                               false,false);
}

void Test_M2MResource::test_copy_constructor()
{
    u_int8_t value[] = {"value"};
    resource->set_value(value,(u_int32_t)sizeof(value));

    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->add_resource_instance(res);

    M2MResource* copy = new M2MResource(*resource);
    u_int8_t* out_value = (u_int8_t*)malloc(sizeof(u_int8_t));
    u_int32_t out_size;

    uint8_t* ptr = (uint8_t*)malloc((uint32_t)sizeof(value));
    m2mresourceinstance_stub::value = ptr;
    memset(m2mresourceinstance_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mresourceinstance_stub::value,value,sizeof(value));
    m2mresourceinstance_stub::int_value = (uint32_t)sizeof(value);

    copy->get_value(out_value,out_size);

    CHECK(out_size == sizeof(value));

    free(out_value);
    free(ptr);

    delete copy;
}

Test_M2MResource::~Test_M2MResource()
{
    delete resource;
    delete callback;
}

void Test_M2MResource::test_static_resource()
{
    u_int8_t value[] = {"value"};
    M2MResource *res = new M2MResource(*callback,
                                       "name",
                                      "resource_type",
                                      M2MResourceInstance::INTEGER,
                                      value,(uint32_t)sizeof(value),
                                      true);

    CHECK(res != NULL);
    delete res;

}

void Test_M2MResource::test_base_type()
{
    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(M2MBase::Resource == resource->base_type());
}

void Test_M2MResource::test_muliptle_instances()
{
    CHECK(false == resource->supports_multiple_instances());
}

void Test_M2MResource::test_handle_observation_attribute()
{
    char *d = "s";

    m2mresourceinstance_stub::resource_type = M2MResourceInstance::INTEGER;
    CHECK(false == resource->handle_observation_attribute(d));

    m2mresourceinstance_stub::resource_type = M2MResourceInstance::FLOAT;
    CHECK(false == resource->handle_observation_attribute(d));

    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::INTEGER,*callback);
    resource->add_resource_instance(res);

    CHECK(false == resource->handle_observation_attribute(d));
}

void Test_M2MResource::test_add_resource_instance()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->add_resource_instance(res);
    CHECK(resource->_resource_instance_list.size() == 1);
}

void Test_M2MResource::test_remove_resource_instance()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->_resource_instance_list.push_back(res);
    CHECK(resource->remove_resource_instance(0) == true);
}

void Test_M2MResource::test_resource_instance()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->_resource_instance_list.push_back(res);
    CHECK(resource->resource_instance(0) != NULL);
}

void Test_M2MResource::test_resource_instances()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->_resource_instance_list.push_back(res);
    M2MResourceInstanceList list = resource->resource_instances();
    CHECK(list.size() ==1);
}

void Test_M2MResource::test_resource_instance_count()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->_resource_instance_list.push_back(res);
    CHECK(resource->resource_instance_count() ==1);
}

void Test_M2MResource::test_add_observation_level()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->_resource_instance_list.push_back(res);

    M2MBase::Observation obs_level = M2MBase::R_Attribute;

    resource->add_observation_level(obs_level);
}

void Test_M2MResource::test_remove_observation_level()
{
    M2MResourceInstance *res = new M2MResourceInstance("name","type",M2MResourceInstance::STRING,*callback);
    resource->_resource_instance_list.push_back(res);

    M2MBase::Observation obs_level = M2MBase::R_Attribute;

    resource->remove_observation_level(obs_level);
}
