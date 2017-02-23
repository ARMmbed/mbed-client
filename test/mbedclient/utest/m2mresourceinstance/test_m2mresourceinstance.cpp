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
#include "m2mreporthandler_stub.h"
#include "common_stub.h"
#include "m2mtlvdeserializer_stub.h"
#include "m2mblockmessage_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mobject_stub.h"
#include "m2mresource_stub.h"

static bool cb_visited = false;
static void callback_function(void *args)
{
    cb_visited = true;
}
static void global_notification_sent_function()
{
    cb_visited = true;
}

class MyTest{
public:
    void execute_function(void */*argument*/) {
        visited = true;
    }
    void block_message_received(M2MBlockMessage */*argument*/) {
        block_received = true;
    }
    void block_message_requested(const String& /*resource*/, uint8_t *&/*data*/, uint32_t &/*len*/) {
        block_requested = true;
    }
    void notification_sent() {
        visited = true;
    }

    bool visited;
    bool block_received;
    bool block_requested;
};

class ResourceCallback : public M2MResourceCallback {

public:

    ResourceCallback() : visited(false) {}
    ~ResourceCallback(){}
    void notification_update() {
        visited = true;
    }

    void clear() {visited = false;}
    bool visited;
};

class TestReportObserver :  public M2MReportObserver{
public :
    TestReportObserver() {}
    ~TestReportObserver() {}
    void observation_to_be_sent(m2m::Vector<uint16_t>,bool){ }
};

class Handler : public M2MObservationHandler {

public:

    Handler(){}
    ~Handler(){}
    void observation_to_be_sent(M2MBase *, uint16_t, m2m::Vector<uint16_t>, bool){
        visited = true;
    }
    void send_delayed_response(M2MBase *){}
    void resource_to_be_deleted(M2MBase *){visited=true;}
    void remove_object(M2MBase *){visited = true;}
    void value_updated(M2MBase *,const String&){visited = true;}

    void clear() {visited = false;}
    bool visited;
};

Test_M2MResourceInstance::Test_M2MResourceInstance()
{
    handler = new Handler();
    obj = new M2MObject("name", "name");
    m2mresource_stub::object_instance = new M2MObjectInstance(*obj, "name", "", "");
    m2mobjectinstance_stub::resource = new M2MResource(*m2mresource_stub::object_instance,
                                                       "name",
                                                       "type",
                                                       M2MResourceInstance::STRING,
                                                       false,
                                                       "name");

    resource_instance = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                          "name",
                          "resource_type",
                          M2MResourceInstance::STRING,
                          0,
                          "name",
                          false);
}

Test_M2MResourceInstance::~Test_M2MResourceInstance()
{
    delete m2mresource_stub::object_instance;
    delete m2mobjectinstance_stub::resource;
    delete resource_instance;
    delete obj;
    delete handler;
}

void Test_M2MResourceInstance::test_resource_instance_type()
{
    CHECK(resource_instance->resource_instance_type() == M2MResourceInstance::STRING);
}

void Test_M2MResourceInstance::test_static_resource_instance()
{
    u_int8_t value[] = {"value"};
    m2mbase_stub::mode_value = M2MBase::Dynamic;
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                       "name1",
                                                       "type1",
                                                       M2MResourceInstance::INTEGER,
                                                       value, (uint32_t)sizeof(value),
                                                       0,
                                                       "name1",
                                                       false);

    CHECK(res != NULL);
    delete res;
    res = NULL;

    m2mbase_stub::mode_value = M2MBase::Static;
    m2mbase_stub::nsdl_resource = (sn_nsdl_dynamic_resource_parameters_s*)malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*)malloc(sizeof(sn_nsdl_static_resource_parameters_s));

    res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                       "name1",
                                                       "type1",
                                                       M2MResourceInstance::INTEGER,
                                                       value, (uint32_t)sizeof(value),
                                                       0,
                                                       "name1",
                                                       false);

    free(m2mbase_stub::nsdl_resource->static_resource_parameters->resource);
    free(m2mbase_stub::nsdl_resource->static_resource_parameters);
    free(m2mbase_stub::nsdl_resource);
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
    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);

    CHECK(false == resource_instance->handle_observation_attribute(d));

    resource_instance->_resource_type = M2MResourceInstance::INTEGER;
    CHECK(false == resource_instance->handle_observation_attribute(d));

    resource_instance->_resource_type = M2MResourceInstance::FLOAT;
    CHECK(false == resource_instance->handle_observation_attribute(d));

    m2mreporthandler_stub::bool_return = true;
    CHECK(true == resource_instance->handle_observation_attribute(d));

    m2mbase_stub::bool_value = false;
    CHECK(true == resource_instance->handle_observation_attribute(d));

    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
}

void Test_M2MResourceInstance::test_set_execute_function()
{
    MyTest test;
    resource_instance->set_execute_function(execute_callback(&test,&MyTest::execute_function));
    resource_instance->set_execute_function(callback_function);
}

void Test_M2MResourceInstance::test_execute()
{
    MyTest test;
    void *args = NULL;

    resource_instance->set_execute_function(execute_callback(&test,&MyTest::execute_function));
    resource_instance->execute(args);

    cb_visited = false;
    resource_instance->set_execute_function(callback_function);
    resource_instance->execute(args);
    CHECK(true == cb_visited);

    // Check delete
    cb_visited = false;
    resource_instance->set_execute_function(callback_function);
    resource_instance->execute(args);
    CHECK(true == cb_visited);
}

void Test_M2MResourceInstance::test_set_value()
{
    u_int8_t value[] = {"value2"};
    resource_instance->_value = (u_int8_t*)malloc(sizeof(u_int8_t));
    m2mbase_stub::bool_value = true;

    CHECK(resource_instance->set_value(value,(u_int32_t)sizeof(value)) == true);
    CHECK( resource_instance->_value_length == sizeof(value));
    CHECK( *resource_instance->_value == *value);

    m2mbase_stub::observe = (M2MObservationHandler*)handler;

    u_int8_t value2[] = {"12"};
    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);

    u_int8_t value3[] = {"13"};
    CHECK(resource_instance->set_value(value3,(u_int32_t)sizeof(value3)) == true);

    CHECK(resource_instance->set_value(123456789) == true);
    CHECK(memcmp(resource_instance->_value, "123456789", 9) == 0);

    // verify int value helper
    CHECK(resource_instance->get_value_int() == 123456789);

    // verify string value helper
    CHECK(resource_instance->get_value_string() == "123456789");

    free(resource_instance->_value);
    resource_instance->_value_length = 0;

    CHECK(resource_instance->set_value(NULL,0) == false);

    CHECK(resource_instance->set_value(NULL,0) == false);

    m2mbase_stub::observation_level_value = M2MBase::R_Attribute;
    resource_instance->_value = (u_int8_t*)malloc(sizeof(value)+1);
    memset(resource_instance->_value,0,sizeof(value)+1);
    memcpy(resource_instance->_value,value,sizeof(value));
    resource_instance->_value_length = sizeof(value);
    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);

    u_int8_t value4[] = {"value4"};
    CHECK(resource_instance->set_value(value4,(u_int32_t)sizeof(value4)) == true);


    m2mbase_stub::base_type = M2MBase::ResourceInstance;
    m2mbase_stub::observation_level_value = M2MBase::O_Attribute;
    resource_instance->_resource_type = M2MResourceInstance::INTEGER;
    m2mbase_stub::mode_value = M2MBase::Dynamic;
    ResourceCallback *resource_cb = new ResourceCallback();
    resource_instance->set_resource_observer(resource_cb);
    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);

    // XXX: the callback will not be called on current code with combination of
    // M2MBase::Dynamic and M2MBase::R_Attribute.
    CHECK(resource_cb->visited == false);

    resource_cb->visited = false;
    m2mbase_stub::observation_level_value = M2MBase::R_Attribute;
    CHECK(resource_instance->set_value(value3,(u_int32_t)sizeof(value3)) == true);
    CHECK(resource_cb->visited == true);

    resource_instance->set_resource_observer(NULL);
    resource_cb->visited = false;
    m2mbase_stub::observation_level_value = M2MBase::R_Attribute;
    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);
    CHECK(resource_cb->visited == false);


    CHECK(resource_instance->set_value(value3,(u_int32_t)sizeof(value3)) == true);

    m2mbase_stub::observation_level_value = M2MBase::OI_Attribute;

    resource_instance->_resource_type = M2MResourceInstance::INTEGER;

    m2mbase_stub::mode_value = M2MBase::Dynamic;

    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);

    m2mbase_stub::observation_level_value = M2MBase::OOI_Attribute;

    resource_instance->_resource_type = M2MResourceInstance::INTEGER;

    m2mbase_stub::mode_value = M2MBase::Dynamic;

    CHECK(resource_instance->set_value(value2,(u_int32_t)sizeof(value2)) == true);

    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
    delete resource_cb;
}

void Test_M2MResourceInstance::test_clear_value()
{
    u_int8_t value[] = {"value"};
    resource_instance->_value = (u_int8_t*)malloc(sizeof(u_int8_t));

    m2mbase_stub::observe = handler;
    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);

    CHECK(resource_instance->set_value(value,(u_int32_t)sizeof(value)) == true);
    CHECK( resource_instance->_value_length == sizeof(value));
    CHECK( *resource_instance->_value == *value);
    resource_instance->clear_value();

    CHECK( resource_instance->_value_length == 0);
    CHECK( resource_instance->_value == NULL);

    m2mbase_stub::bool_value = true;
    m2mbase_stub::mode_value = M2MBase::Dynamic;
    m2mbase_stub::observation_level_value = M2MBase::R_Attribute;
    resource_instance->_resource_type = M2MResourceInstance::INTEGER;
    resource_instance->clear_value();

    CHECK( resource_instance->_value_length == 0);
    CHECK( resource_instance->_value == NULL);

    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
}

void Test_M2MResourceInstance::test_get_value()
{
    u_int8_t test_value[] = {"value3"};
    u_int32_t value_length((u_int32_t)sizeof(test_value));

    resource_instance->_value = (u_int8_t *)malloc(value_length);
    resource_instance->_value_length = value_length;
    memcpy((u_int8_t *)resource_instance->_value, (u_int8_t *)test_value, value_length);

    uint8_t* buffer = (uint8_t*)malloc(5);
    uint32_t val_size = 0;
    resource_instance->get_value(buffer, val_size);
    CHECK(val_size == value_length);
    free(buffer);

    resource_instance->clear_value();
    CHECK(resource_instance->_value == NULL);


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
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    m2mbase_stub::operation = M2MBase::GET_ALLOWED;
    m2mbase_stub::uint8_value = 200;
    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->token_ptr = (uint8_t*)malloc(sizeof(value));
    memcpy(coap_header->token_ptr, value, sizeof(value));

    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->observe = 0;

    coap_header->content_format = sn_coap_content_format_e(110);

    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    MyTest test;
    test.block_requested = false;
    resource_instance->set_incoming_block_message_callback(
                incoming_block_message_callback(&test, &MyTest::block_message_received));
    resource_instance->set_outgoing_block_message_callback(
        outgoing_block_message_callback(&test,&MyTest::block_message_requested));
    m2mblockmessage_stub::is_block_message = true;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);
    CHECK(test.block_requested == true);
    m2mblockmessage_stub::is_block_message = false;

    // OMA OPAQUE
    resource_instance->_resource_type = M2MResourceInstance::OPAQUE;

    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    // Not OMA TLV or JSON
    m2mbase_stub::uint8_value = 110;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    // OMA TLV
    m2mbase_stub::uint8_value = 99;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);
    // OMA JSON
    m2mbase_stub::uint8_value = 100;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    coap_header->options_list_ptr->observe = 0;
    m2mbase_stub::uint16_value = 0x1c1c;
    m2mbase_stub::bool_value = true;

    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    // Not observable
    m2mbase_stub::bool_value = false;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::bool_value = true;

    coap_header->options_list_ptr->observe = 0;

    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    coap_header->options_list_ptr->observe = 1;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;
    CHECK(resource_instance->handle_get_request(NULL,coap_header,handler) != NULL);

    CHECK(resource_instance->handle_get_request(NULL,NULL,handler) != NULL);

    if(coap_header->token_ptr) {
        free(coap_header->token_ptr);
        coap_header->token_ptr = NULL;
    }
    if(coap_header->options_list_ptr) {
        free(coap_header->options_list_ptr);
        coap_header->options_list_ptr = NULL;
    }

    if(common_stub::coap_header){
        if(common_stub::coap_header->options_list_ptr) {
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    coap_header = NULL;

    m2mbase_stub::clear();
    common_stub::clear();
}

void Test_M2MResourceInstance::test_handle_put_request()
{
    uint8_t value[] = {"name"};
    bool execute_value_updated = false;
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;
    m2mbase_stub::uint8_value = 200;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->payload_ptr = (uint8_t*)malloc(1);

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->uri_query_ptr = value;
    coap_header->options_list_ptr->uri_query_len = sizeof(value);

    coap_header->content_format = sn_coap_content_format_e(99);
    m2mtlvdeserializer_stub::bool_value = true;

    m2mbase_stub::bool_value = false;

    sn_coap_hdr_s *coap_response = NULL;
    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK( coap_response != NULL);

    free(coap_header->options_list_ptr);
    coap_header->options_list_ptr = NULL;

    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK( coap_response != NULL);

    m2mtlvdeserializer_stub::bool_value = false;

    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    coap_header->content_format = sn_coap_content_format_e(100);

    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    m2mbase_stub::bool_value = true;

    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;

    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    coap_response = resource_instance->handle_put_request(NULL,NULL,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    MyTest test;
    test.block_received = false;
    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;
    m2mblockmessage_stub::is_block_message = true;
    m2mblockmessage_stub::is_last_block = false;
    resource_instance->set_incoming_block_message_callback(
                incoming_block_message_callback(&test, &MyTest::block_message_received));
    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK(test.block_received == true);
    CHECK( coap_response != NULL);

    test.block_received = false;
    m2mblockmessage_stub::is_block_message = true;
    m2mblockmessage_stub::is_last_block = true;
    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK(test.block_received == true);
    CHECK( coap_response != NULL);

    test.block_received = false;
    m2mblockmessage_stub::is_block_message = false;
    m2mblockmessage_stub::is_last_block = false;
    coap_response = resource_instance->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK(test.block_received == false);
    CHECK( coap_response != NULL);

    free(coap_header->options_list_ptr);
    free(coap_header->payload_ptr);
    free(common_stub::coap_header);
    free(coap_header);

    m2mtlvdeserializer_stub::clear();
    common_stub::clear();
    m2mbase_stub::clear();
}

void Test_M2MResourceInstance::test_set_resource_observer()
{
    ResourceCallback *resource_cb = new ResourceCallback();
    resource_instance->set_resource_observer(resource_cb);
    CHECK(resource_instance->_resource_callback == resource_cb)
    delete resource_cb;
}


void Test_M2MResourceInstance::test_get_object_name()
{
    m2mbase_stub::string_value = "name";
    CHECK(resource_instance->object_name() == "name");
}

void Test_M2MResourceInstance::test_get_object_instance_id()
{
    resource_instance->_object_instance_id = 100;
    CHECK(resource_instance->object_instance_id() == 100);
}

void Test_M2MResourceInstance::test_set_notification_sent_function()
{
    MyTest test;
    resource_instance->set_notification_sent_callback(
                notification_sent_callback(&test,&MyTest::notification_sent));
    resource_instance->set_notification_sent_callback(global_notification_sent_function);
}

void Test_M2MResourceInstance::test_notification_sent()
{
    MyTest test;
    void *args = NULL;

    resource_instance->set_notification_sent_callback(
                notification_sent_callback(&test,&MyTest::notification_sent));
    resource_instance->notification_sent();

    cb_visited = false;
    resource_instance->set_notification_sent_callback(global_notification_sent_function);
    resource_instance->notification_sent();
    CHECK(true == cb_visited);

    // Check delete
    cb_visited = false;
    resource_instance->set_notification_sent_callback(global_notification_sent_function);
    resource_instance->notification_sent();
    CHECK(true == cb_visited);
}

void Test_M2MResourceInstance::test_ctor()
{
    M2MResourceInstance* instance = new M2MResourceInstance(*m2mobjectinstance_stub::resource, &params,
                                                            M2MResourceInstance::STRING, 0);
    CHECK(instance != NULL);
    delete instance;
}

void Test_M2MResourceInstance::test_get_parent_resource()
{
    M2MResourceInstance* instance = new M2MResourceInstance(*m2mobjectinstance_stub::resource, &params,
                                                            M2MResourceInstance::STRING, 0);
    // Only for the code coverage
    instance->get_parent_resource();
    delete instance;
}
