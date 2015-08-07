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

class MyTest{
public:
    void execute_function(void */*argument*/) {
        visited = true;
    }

    bool visited;
};

Test_M2MResource::Test_M2MResource()
{
    resource = new M2MResource("name",
                               "resource_type",
                               M2MResourceInstance::INTEGER,
                               false,false);
}

void Test_M2MResource::test_copy_constructor()
{
//    u_int8_t value[] = {"value"};
//    resource->set_value(value,(u_int32_t)sizeof(value));

//    M2MResource* copy = new M2MResource(*resource);
//    u_int8_t* out_value = (u_int8_t*)malloc(sizeof(u_int8_t));
//    u_int32_t out_size;

//    uint8_t* ptr = (uint8_t*)malloc((uint32_t)sizeof(value));
//    m2mbase_stub::value = ptr;
//    memset(m2mbase_stub::value,0,(uint32_t)sizeof(value));
//    memcpy(m2mbase_stub::value,value,sizeof(value));
//    m2mbase_stub::int_value = (uint32_t)sizeof(value);

//    copy->get_value(out_value,out_size);

//    CHECK(out_size == sizeof(value));

//    free(out_value);
//    free(ptr);

//    delete copy;
}

Test_M2MResource::~Test_M2MResource()
{
    delete resource;
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
    CHECK(false == resource->handle_observation_attribute(d));
}

void Test_M2MResource::test_add_resource_instance()
{

}

void Test_M2MResource::test_remove_resource_instance()
{

}

void Test_M2MResource::test_resource_instance()
{

}

void Test_M2MResource::test_resource_instances()
{

}

void Test_M2MResource::test_resource_instance_count()
{

}
