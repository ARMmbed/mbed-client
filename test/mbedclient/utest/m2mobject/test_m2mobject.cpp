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
#include "test_m2mobject.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mbase_stub.h"
#include "common_stub.h"
#include "m2mtlvdeserializer_stub.h"

class Handler : public M2MObservationHandler {

public:

    Handler(){}
    ~Handler(){}
    void observation_to_be_sent(M2MBase *){
        visited = true;
    }
    void resource_to_be_deleted(const String &){visited=true;}
    void remove_object(M2MBase *){visited = true;}
    void value_updated(M2MBase *){visited = true;}

    void clear() {visited = false;}
    bool visited;
};

Test_M2MObject::Test_M2MObject()
{
    handler = new Handler();
    object = new M2MObject("name");
}

Test_M2MObject::~Test_M2MObject()
{
    m2mobjectinstance_stub::clear();
    m2mresource_stub::clear();
    m2mbase_stub::clear();
    m2mbase_stub::string_value = new String("name");
    delete object;
    delete handler;
    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;
}

void Test_M2MObject::test_copy_constructor()
{
    String *name = new String("name");
    m2mbase_stub::string_value = name;

    M2MObject* copy = new M2MObject(*name);
    M2MObjectInstance *ins = new M2MObjectInstance("name");
    copy->set_instance_id(0);
    copy->_instance_list.push_back(ins);

    M2MObject* copy1 = new M2MObject(*copy);

    CHECK(1 == copy1->_instance_list.size());

    delete copy;
    delete copy1;
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

    CHECK(true == object->remove_object_instance(0));

    CHECK(false == object->remove_object_instance(0));
}

void Test_M2MObject::test_object_instance()
{
    String *test = new String("name");
    M2MObjectInstance *ins = new M2MObjectInstance(*test);
    object->set_instance_id(0);
    object->_instance_list.push_back(ins);

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

    M2MObjectInstance *ins1 = new M2MObjectInstance(*test);
    ins1->set_instance_id(1);
    object->_instance_list.push_back(ins1);

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

    M2MObjectInstance *ins1 = new M2MObjectInstance(test);
    object->set_instance_id(1);
    object->_instance_list.push_back(ins1);

    CHECK(2 == object->instance_count());

    object->_instance_list.clear();
    delete ins;
    delete ins1;
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

    m2mobjectinstance_stub::bool_value = true;
    ret = object->handle_observation_attribute(c);
    CHECK(ret == true);
}

void Test_M2MObject::test_handle_get_request()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;

    m2mbase_stub::operation = M2MBase::GET_ALLOWED;
    m2mbase_stub::uint8_value = 200;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->token_ptr = (uint8_t*)malloc(sizeof(value));
    memcpy(coap_header->token_ptr, value, sizeof(value));

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->observe = 0;

    coap_header->content_type_ptr = (uint8_t*)malloc(1);
    coap_header->content_type_len = 1;
    *coap_header->content_type_ptr = 110;

    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(coap_header->content_type_ptr) {
        free(coap_header->content_type_ptr);
        coap_header->content_type_ptr = NULL;
    }

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }
    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }

    // Not OMA TLV or JSON
    m2mbase_stub::uint8_value = 110;
    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }
    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }

    // OMA TLV
    m2mbase_stub::uint8_value = 99;
    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }
    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }

    // OMA JSON
    m2mbase_stub::uint8_value = 100;
    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }
    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }

    coap_header->options_list_ptr->observe = 1;

    uint8_t obs = 0;
    coap_header->options_list_ptr->observe_ptr = (uint8_t*)malloc(sizeof(obs));
    memcpy(coap_header->options_list_ptr->observe_ptr,&obs,sizeof(obs));
    coap_header->options_list_ptr->observe_len = 0;
    m2mbase_stub::uint16_value = 0x1c1c;

    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }

    if(common_stub::coap_header->options_list_ptr->observe_ptr) {
        free(common_stub::coap_header->options_list_ptr->observe_ptr);
        common_stub::coap_header->options_list_ptr->observe_ptr = NULL;
    }

    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }

    coap_header->options_list_ptr->observe_len = 1;

    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }

    if(common_stub::coap_header->options_list_ptr->observe_ptr) {
        free(common_stub::coap_header->options_list_ptr->observe_ptr);
        common_stub::coap_header->options_list_ptr->observe_ptr = NULL;
    }

    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }


    obs = 1;
    memcpy(coap_header->options_list_ptr->observe_ptr,&obs,sizeof(obs));
    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    if(common_stub::coap_header->content_type_ptr) {
        free(common_stub::coap_header->content_type_ptr);
        common_stub::coap_header->content_type_ptr = NULL;
    }

    if(common_stub::coap_header->options_list_ptr->observe_ptr) {
        free(common_stub::coap_header->options_list_ptr->observe_ptr);
        common_stub::coap_header->options_list_ptr->observe_ptr = NULL;
    }

    if(common_stub::coap_header->options_list_ptr) {
        free(common_stub::coap_header->options_list_ptr);
        common_stub::coap_header->options_list_ptr = NULL;
    }

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;
    CHECK(object->handle_get_request(NULL,coap_header,handler) != NULL);

    CHECK(object->handle_post_request(NULL,NULL,handler) != NULL);

    if(coap_header->token_ptr) {
        free(coap_header->token_ptr);
        coap_header->token_ptr = NULL;
    }
    if(coap_header->content_type_ptr) {
        free(coap_header->content_type_ptr);
        coap_header->content_type_ptr = NULL;
    }
    if(coap_header->options_list_ptr->observe_ptr) {
        free(coap_header->options_list_ptr->observe_ptr);
        coap_header->options_list_ptr->observe_ptr = NULL;
    }
    if(coap_header->options_list_ptr) {
        free(coap_header->options_list_ptr);
        coap_header->options_list_ptr = NULL;
    }

    if(common_stub::coap_header){
        if(common_stub::coap_header->content_type_ptr) {
            free(common_stub::coap_header->content_type_ptr);
            common_stub::coap_header->content_type_ptr = NULL;
        }
        if(common_stub::coap_header->options_list_ptr) {
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    coap_header = NULL;

    delete name;
    name = NULL;

    m2mbase_stub::clear();
    common_stub::clear();
}

void Test_M2MObject::test_handle_put_request()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;

    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;
    m2mbase_stub::uint8_value = 200;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->payload_ptr = (uint8_t*)malloc(1);

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->uri_query_ptr = value;
    coap_header->options_list_ptr->uri_query_len = sizeof(value);

    coap_header->content_type_ptr = (uint8_t*)malloc(1);
    coap_header->content_type_len = 1;
    *coap_header->content_type_ptr = 99;
    m2mtlvdeserializer_stub::bool_value = true;

    m2mbase_stub::bool_value = false;

    CHECK(object->handle_put_request(NULL,coap_header,handler) != NULL);

    m2mtlvdeserializer_stub::bool_value = false;

    CHECK(object->handle_put_request(NULL,coap_header,handler) != NULL);

    *coap_header->content_type_ptr = 100;

    CHECK(object->handle_put_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::bool_value = true;

    CHECK(object->handle_put_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;

    CHECK(object->handle_put_request(NULL,coap_header,handler) != NULL);

    CHECK(object->handle_post_request(NULL,NULL,handler) != NULL);

    free(coap_header->content_type_ptr);
    free(coap_header->options_list_ptr);
    free(coap_header->payload_ptr);
    free(common_stub::coap_header);
    delete name;
    free(coap_header);

    m2mtlvdeserializer_stub::clear();
    common_stub::clear();
    m2mbase_stub::clear();
}

void Test_M2MObject::test_handle_post_request()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;

    m2mbase_stub::operation = M2MBase::POST_ALLOWED;
    m2mbase_stub::uint8_value = 200;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->payload_ptr = (uint8_t*)malloc(1);

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->uri_query_ptr = value;
    coap_header->options_list_ptr->uri_query_len = sizeof(value);

    coap_header->content_type_ptr = (uint8_t*)malloc(1);
    coap_header->content_type_len = 1;
    *coap_header->content_type_ptr = 99;
    m2mtlvdeserializer_stub::bool_value = true;

    m2mbase_stub::bool_value = false;

    CHECK(object->handle_post_request(NULL,coap_header,handler) != NULL);

    m2mtlvdeserializer_stub::bool_value = false;

    CHECK(object->handle_post_request(NULL,coap_header,handler) != NULL);

    *coap_header->content_type_ptr = 100;

    CHECK(object->handle_post_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::bool_value = true;

    CHECK(object->handle_post_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;

    CHECK(object->handle_post_request(NULL,coap_header,handler) != NULL);

    CHECK(object->handle_post_request(NULL,NULL,handler) != NULL);

    free(coap_header->content_type_ptr);
    free(coap_header->options_list_ptr);
    free(coap_header->payload_ptr);
    free(common_stub::coap_header);
    delete name;
    free(coap_header);

    m2mtlvdeserializer_stub::clear();
    common_stub::clear();
    m2mbase_stub::clear();
}
