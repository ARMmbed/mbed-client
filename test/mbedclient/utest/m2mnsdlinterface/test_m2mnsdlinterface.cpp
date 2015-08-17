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
#include "test_m2mnsdlinterface.h"
#include "common_stub.h"
#include "m2mnsdlobserver.h"
#include "m2mobject_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mresource.h"
#include "m2mbase_stub.h"
#include "m2mserver.h"
#include "m2msecurity.h"

class TestObserver : public M2MNsdlObserver {

public:
    TestObserver(){}
    virtual ~TestObserver(){}
    void coap_message_ready(uint8_t *,
                            uint16_t,
                            sn_nsdl_addr_s *){
        message_ready = true;
    }

    void client_registered(M2MServer *){
        registered = true;
    }

    void registration_updated(const M2MServer &){
        register_updated = true;
    }

    void registration_error(uint8_t){
        register_error = true;
    }

    void client_unregistered(){
        unregistered = true;
    }

    void bootstrap_done(M2MSecurity *sec){
        if(sec) {
            boot_done = true;
            delete sec;
            sec = NULL;
        }
    }

    void bootstrap_error(){
        boot_error = true;
    }

    void coap_data_processed(){
        data_processed = true;
    }

    void value_updated(M2MBase *){
    }

    bool register_error;
    bool boot_error;
    bool boot_done;
    bool registered;
    bool register_updated;
    bool data_processed;
    bool unregistered;
    bool message_ready;
};

Test_M2MNsdlInterface::Test_M2MNsdlInterface()
{
    observer = new TestObserver();
    nsdl = new M2MNsdlInterface(*observer);
    nsdl->_server = new M2MServer();
}

Test_M2MNsdlInterface:: ~Test_M2MNsdlInterface()
{
    delete nsdl;
    nsdl = NULL;
    delete observer;
    observer = NULL;
}

void Test_M2MNsdlInterface::test_create_endpoint()
{
    u_int8_t value[]  = {"120"};
    if( nsdl->_endpoint == NULL){
        nsdl->_endpoint = (sn_nsdl_ep_parameters_s*)nsdl->memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }

    nsdl->create_endpoint("name", "type",120,"domain",100,"context");
    CHECK(nsdl->_endpoint->lifetime_len == 3);
    CHECK(*nsdl->_endpoint->lifetime_ptr == *value);
}

void Test_M2MNsdlInterface::test_delete_endpoint()
{
    if( nsdl->_endpoint == NULL){
        nsdl->_endpoint = (sn_nsdl_ep_parameters_s*)nsdl->memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(uint8_t));

    nsdl->delete_endpoint();
    CHECK(nsdl->_endpoint == NULL);
}

void Test_M2MNsdlInterface::test_create_nsdl_list_structure()
{
    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::int_value = 0;
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name);

    M2MResource* create_resource = new M2MResource(*name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,
                                                   false);

    m2mobject_stub::instance_list.clear();
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.clear();
    m2mobjectinstance_stub::resource_list.push_back(create_resource);

    M2MObjectList list;
    list.push_back(object);

    m2mbase_stub::string_value = name;
    m2mbase_stub::mode_value = M2MBase::Static;

    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    m2mresource_stub::bool_value = true;
    m2mbase_stub::mode_value = M2MBase::Dynamic;
    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    nsdl->_resource->path = ((uint8_t*)nsdl->memory_alloc(3));
    m2mbase_stub::mode_value = M2MBase::Directory;
    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    list.clear();
    delete object;
    delete name;
    name = NULL;
    m2mobject_stub::instance_list.clear();
    delete instance;
    instance = NULL;
    m2mobjectinstance_stub::resource_list.clear();
    delete create_resource;
    create_resource = NULL;    
}

void Test_M2MNsdlInterface::test_delete_nsdl_resource()
{
    common_stub::int_value = -1;

    CHECK(nsdl->delete_nsdl_resource("name") == false);

    common_stub::int_value = 0;

    CHECK(nsdl->delete_nsdl_resource("name") == true);
}

void Test_M2MNsdlInterface::test_create_bootstrap_resource()
{
    common_stub::uint_value = 11;
    CHECK(nsdl->create_bootstrap_resource(NULL) == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->create_bootstrap_resource(NULL) == false);
}

void Test_M2MNsdlInterface::test_send_register_message()
{
    common_stub::uint_value = 12;
    CHECK(nsdl->send_register_message(NULL,100,SN_NSDL_ADDRESS_TYPE_IPV6) == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->send_register_message(NULL,100,SN_NSDL_ADDRESS_TYPE_IPV6) == false);
}

void Test_M2MNsdlInterface::test_send_update_registration()
{
    common_stub::uint_value = 23;
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(1));
    CHECK(nsdl->send_update_registration(120) == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->send_update_registration(120) == false);

    free(nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_send_unregister_message()
{
    common_stub::uint_value = 22;
    CHECK(nsdl->send_unregister_message() == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->send_unregister_message() == false);

}

void Test_M2MNsdlInterface::test_memory_alloc()
{
    CHECK(nsdl->memory_alloc(0) == 0);
    uint8_t *ptr = 0;
    ptr = (uint8_t*)nsdl->memory_alloc(sizeof(uint8_t));
    CHECK(ptr != NULL);
    nsdl->memory_free(ptr);    
}

void Test_M2MNsdlInterface::test_memory_free()
{
    uint8_t *ptr = (uint8_t*)nsdl->memory_alloc(sizeof(uint8_t));
    nsdl->memory_free((void*)ptr);
    //memory leak test will fail, if there is a leak, so no need for CHECK
}

void Test_M2MNsdlInterface::test_send_to_server_callback()
{
  uint8_t *data_ptr = (uint8_t*)malloc(sizeof(uint8_t));
  uint16_t data_len = sizeof(uint8_t);
  sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));

  nsdl->send_to_server_callback(NULL, SN_NSDL_PROTOCOL_COAP, data_ptr,data_len,address);
  CHECK(observer->message_ready == true);

  free(data_ptr);
  free(address);
}

void Test_M2MNsdlInterface::test_received_from_server_callback()
{
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;


    coap_header->options_list_ptr = (sn_coap_options_list_s *)malloc(sizeof(sn_coap_options_list_s));
    memset(coap_header->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

    coap_header->options_list_ptr->max_age_len = 2;
    coap_header->options_list_ptr->max_age_ptr = (uint8_t *)malloc(sizeof(coap_header->options_list_ptr->max_age_len));
    memset(coap_header->options_list_ptr->max_age_ptr, 0, sizeof(coap_header->options_list_ptr->max_age_len));

    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(coap_header->options_list_ptr->max_age_ptr);
    coap_header->options_list_ptr->max_age_ptr = NULL;

    free(coap_header->options_list_ptr);
    coap_header->options_list_ptr = NULL;

    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    uint8_t life[] = {"120"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(life));
    memcpy(nsdl->_endpoint->lifetime_ptr,life,sizeof(life));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(life);

    observer->data_processed = false;
    observer->registered = false;

    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);
    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    uint8_t big_life[] = {"4000"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(big_life));
    memcpy(nsdl->_endpoint->lifetime_ptr,big_life,sizeof(big_life));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(big_life);
    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    observer->data_processed = false;
    observer->registered = false;

    uint8_t less_life[] = {"30"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(less_life));
    memcpy(nsdl->_endpoint->lifetime_ptr,less_life,sizeof(less_life));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(less_life);

    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    observer->data_processed = false;
    observer->registered = false;
    observer->unregistered = false;
    observer->register_error = false;

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_OPTION;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_INCOMPLETE;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_PRECONDITION_FAILED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_UNAUTHORIZED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_GATEWAY;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    //msg_id == _unregister_id
    coap_header->msg_id = 8;
    nsdl->_unregister_id = 8;

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_DELETED;
    observer->register_error = false;
    nsdl->_server = new M2MServer();
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->unregistered == true);

    observer->register_error = false;
    nsdl->_unregister_id = 8;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;

    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    nsdl->_unregister_id = 8;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    nsdl->_unregister_id = 8;
    coap_header->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->boot_error = false;
    nsdl->_bootstrap_id = 8;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->boot_error == true);

    //_update_id == msg_id
    nsdl->_update_id = 10;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(nsdl->_update_id == 0);
    CHECK(observer->register_updated == true);

    nsdl->_update_id = 10;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    coap_header->coap_status = COAP_STATUS_OK;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(nsdl->_update_id == 0);
    CHECK(observer->register_error == true);

    coap_header->msg_id = 11;
    CHECK( 0== nsdl->received_from_server_callback(NULL,coap_header,NULL) );
    free(coap_header);
}

void Test_M2MNsdlInterface::test_resource_callback_get()
{
    uint8_t value[] = {"name/0/name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address, 0, sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;

    String *name = new String("name/0/name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;
    M2MObject *object = new M2MObject("name");
    M2MObjectInstance* instance = new M2MObjectInstance("name");
    M2MResource* create_resource = new M2MResource("name",
                                                   "name",
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,false);

    m2mobject_stub::instance_list.clear();
    m2mobject_stub::instance_list.push_back(instance);
    m2mobject_stub::int_value = 1;

    m2mobjectinstance_stub::resource_list.clear();
    m2mobjectinstance_stub::resource_list.push_back(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::GET_ALLOWED;
    m2mbase_stub::uint8_value = 200;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->token_ptr = (uint8_t*)malloc(sizeof(value));
    memcpy(coap_header->token_ptr, value, sizeof(value));

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->observe = 0;

    uint8_t obs = 0;
    coap_header->options_list_ptr->observe_ptr = (uint8_t*)malloc(sizeof(obs));
    memcpy(coap_header->options_list_ptr->observe_ptr,&obs,sizeof(obs));

    m2mbase_stub::uint8_value = 1;
    uint8_t value2[] = {"v"};
    m2mresourceinstance_stub::value = (uint8_t *)malloc(sizeof(value2));
    memset( m2mresourceinstance_stub::value, 0, sizeof(value2));
    memcpy(m2mresourceinstance_stub::value,value2,sizeof(value2));
    m2mresourceinstance_stub::int_value = sizeof(value2);

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 0);

    m2mobject_stub::base_type = M2MBase::Resource;
    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 0);

    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 0);

    m2mbase_stub::base_type = M2MBase::Object;

    delete name;
    name = new String("name");
    m2mbase_stub::string_value = name;

    //found case
    m2mbase_stub::int_value = 0;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);

    m2mresource_stub::bool_value = true;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);

    free(coap_header->options_list_ptr->observe_ptr);

    m2mbase_stub::int_value = sizeof(value2);

    coap_header->options_list_ptr->observe = 1;
    obs = 1;
    coap_header->options_list_ptr->observe_ptr = (uint8_t*)malloc(sizeof(obs));
    memcpy(coap_header->options_list_ptr->observe_ptr,&obs,sizeof(obs));

    m2mbase_stub::uint8_value = 0;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==1);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==1);

    delete object;
    m2mobject_stub::instance_list.clear();
    delete instance;
    m2mobjectinstance_stub::resource_list.clear();
    delete create_resource;

    free(coap_header->token_ptr);
    coap_header->token_ptr = NULL;
    free(coap_header->options_list_ptr->observe_ptr);
    coap_header->options_list_ptr->observe_ptr = NULL;
    free(coap_header->options_list_ptr);
    coap_header->options_list_ptr = NULL;

    free(m2mresourceinstance_stub::value);
    m2mresourceinstance_stub::value = NULL;

    if(common_stub::coap_header){
        if( common_stub::coap_header->content_type_ptr ){
            free(common_stub::coap_header->content_type_ptr);
            common_stub::coap_header->content_type_ptr = NULL;
        }
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    coap_header = NULL;
    free(address);


    delete name;
    name = NULL;

    m2mresourceinstance_stub::clear();
    m2mbase_stub::clear();
    common_stub::clear();
    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();
}

void Test_M2MNsdlInterface::test_resource_callback_put()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name);
    M2MResource* create_resource = new M2MResource(*name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,false);
    m2mobject_stub::int_value = 2;
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.push_back(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));

    uint8_t query[] = {"pmax=200&pmin=120"};
    coap_header->options_list_ptr->uri_query_ptr = (uint8_t*)malloc(sizeof(query));
    coap_header->options_list_ptr->uri_query_len = sizeof(query);

    m2mbase_stub::bool_value = true;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::base_type = M2MBase::Resource;
    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Object;
    m2mbase_stub::bool_value = false;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    delete object;
    m2mobject_stub::instance_list.clear();
    delete instance;
//    m2mobjectinstance_stub::resource_list.clear();
    delete create_resource;

    free(coap_header->options_list_ptr->uri_query_ptr);
    free(coap_header->options_list_ptr);
    if(common_stub::coap_header){
        if( common_stub::coap_header->content_type_ptr ){
            free(common_stub::coap_header->content_type_ptr);
            common_stub::coap_header->content_type_ptr = NULL;
        }
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    free(address);

    delete name;
    name = NULL;

    m2mbase_stub::clear();
    common_stub::clear();
    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();
}

void Test_M2MNsdlInterface::test_resource_callback_post()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name);
    M2MResource* create_resource = new M2MResource(*name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,false);
    m2mobject_stub::int_value = 2;
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.push_back(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));

    uint8_t query[] = {"pmax=200&pmin=120"};
    coap_header->options_list_ptr->uri_query_ptr = (uint8_t*)malloc(sizeof(query));
    coap_header->options_list_ptr->uri_query_len = sizeof(query);

    m2mbase_stub::bool_value = true;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::base_type = M2MBase::Resource;
    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Object;
    m2mbase_stub::bool_value = false;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    delete object;
    m2mobject_stub::instance_list.clear();
    delete instance;
    m2mobjectinstance_stub::resource_list.clear();
    delete create_resource;

    free(coap_header->options_list_ptr->uri_query_ptr);
    free(coap_header->options_list_ptr);
    if(common_stub::coap_header){
        if( common_stub::coap_header->content_type_ptr ){
            free(common_stub::coap_header->content_type_ptr);
            common_stub::coap_header->content_type_ptr = NULL;
        }
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    free(address);

    delete name;
    name = NULL;

    m2mbase_stub::clear();
    common_stub::clear();
    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();
}

void Test_M2MNsdlInterface::test_bootstrap_done_callback()
{
    uint8_t server_address[] = {"10.45.3.85"};
    sn_nsdl_oma_server_info_t *server_info =(sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    server_info->omalw_address_ptr = (sn_nsdl_addr_ *)malloc(sizeof(sn_nsdl_addr_));
    server_info->omalw_address_ptr->port = 5685;
    server_info->omalw_address_ptr->addr_ptr = (uint8_t *)malloc(sizeof(server_address));
    memcpy(server_info->omalw_address_ptr->addr_ptr,server_address,sizeof(server_address));
    server_info->omalw_address_ptr->addr_len = sizeof(server_address);
    server_info->omalw_server_security = SEC_NOT_SET;

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = NO_SEC;

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = CERTIFICATE;

    common_stub::cert = (omalw_certificate_list_t *)malloc(sizeof(omalw_certificate_list_t));
    common_stub::cert->certificate_ptr[0] = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->certificate_ptr[1] = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->own_private_key_ptr = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->certificate_len[0] = sizeof(uint8_t);
    common_stub::cert->certificate_len[1] = sizeof(uint8_t);
    common_stub::cert->own_private_key_len = sizeof(uint8_t);

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    observer->boot_done = false;
    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_IPV4;
    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = SEC_NOT_SET;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = PSK;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = RPK;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = NO_SEC;
    nsdl->bootstrap_done_callback(server_info);

    free(common_stub::cert->own_private_key_ptr);
    common_stub::cert->own_private_key_ptr = NULL;
    free(common_stub::cert->certificate_ptr[1]);
    common_stub::cert->certificate_ptr[1] = NULL;
    free(common_stub::cert->certificate_ptr[0]);
    common_stub::cert->certificate_ptr[0] = NULL;
    free(common_stub::cert);
    common_stub::cert = NULL;

    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_IPV4;
    server_info->omalw_server_security = CERTIFICATE;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_NONE;
    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_error == true);

    free(server_info->omalw_address_ptr->addr_ptr);
    server_info->omalw_address_ptr->addr_ptr = NULL;
    free(server_info->omalw_address_ptr);
    server_info->omalw_address_ptr = NULL;
    free(server_info);
    server_info = NULL;

    common_stub::clear();

    // Test for Hostname
    uint8_t hostname_address[] = {"abc.xyz.sss.sdsd"};
    server_info =(sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    server_info->omalw_address_ptr = (sn_nsdl_addr_ *)malloc(sizeof(sn_nsdl_addr_));
    server_info->omalw_address_ptr->port = 5685;
    server_info->omalw_address_ptr->addr_ptr = (uint8_t *)malloc(sizeof(hostname_address));
    memcpy(server_info->omalw_address_ptr->addr_ptr,hostname_address,sizeof(hostname_address));
    server_info->omalw_address_ptr->addr_len = sizeof(hostname_address);
    server_info->omalw_server_security = SEC_NOT_SET;

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = NO_SEC;

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = CERTIFICATE;

    common_stub::cert = (omalw_certificate_list_t *)malloc(sizeof(omalw_certificate_list_t));
    common_stub::cert->certificate_ptr[0] = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->certificate_ptr[1] = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->own_private_key_ptr = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->certificate_len[0] = sizeof(uint8_t);
    common_stub::cert->certificate_len[1] = sizeof(uint8_t);
    common_stub::cert->own_private_key_len = sizeof(uint8_t);

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    observer->boot_done = false;
    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_HOSTNAME;
    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = SEC_NOT_SET;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = PSK;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = RPK;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = NO_SEC;
    nsdl->bootstrap_done_callback(server_info);

    free(common_stub::cert->own_private_key_ptr);
    common_stub::cert->own_private_key_ptr = NULL;
    free(common_stub::cert->certificate_ptr[1]);
    common_stub::cert->certificate_ptr[1] = NULL;
    free(common_stub::cert->certificate_ptr[0]);
    common_stub::cert->certificate_ptr[0] = NULL;
    free(common_stub::cert);
    common_stub::cert = NULL;

    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_HOSTNAME;
    server_info->omalw_server_security = CERTIFICATE;
    nsdl->bootstrap_done_callback(server_info);

    free(server_info->omalw_address_ptr->addr_ptr);
    server_info->omalw_address_ptr->addr_ptr = NULL;
    free(server_info->omalw_address_ptr);
    server_info->omalw_address_ptr = NULL;
    free(server_info);
    server_info = NULL;

    common_stub::clear();

    //Test for IPv6 address
    uint8_t ipv6_address[] = {"FD00:FF1:CE0B:A5E1:1068:AF13:9B61:D557"};
    server_info =(sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    server_info->omalw_address_ptr = (sn_nsdl_addr_ *)malloc(sizeof(sn_nsdl_addr_));
    server_info->omalw_address_ptr->port = 5685;
    server_info->omalw_address_ptr->addr_ptr = (uint8_t *)malloc(sizeof(ipv6_address));
    memcpy(server_info->omalw_address_ptr->addr_ptr,ipv6_address,sizeof(ipv6_address));
    server_info->omalw_address_ptr->addr_len = sizeof(ipv6_address);
    server_info->omalw_server_security = SEC_NOT_SET;

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = NO_SEC;

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = CERTIFICATE;

    common_stub::cert = (omalw_certificate_list_t *)malloc(sizeof(omalw_certificate_list_t));
    common_stub::cert->certificate_ptr[0] = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->certificate_ptr[1] = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->own_private_key_ptr = (uint8_t *)malloc(sizeof(uint8_t));
    common_stub::cert->certificate_len[0] = sizeof(uint8_t);
    common_stub::cert->certificate_len[1] = sizeof(uint8_t);
    common_stub::cert->own_private_key_len = sizeof(uint8_t);

    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    observer->boot_done = false;
    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_IPV6;
    nsdl->bootstrap_done_callback(server_info);
    CHECK(observer->boot_done == true);

    server_info->omalw_server_security = SEC_NOT_SET;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = PSK;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = RPK;
    nsdl->bootstrap_done_callback(server_info);

    server_info->omalw_server_security = NO_SEC;
    nsdl->bootstrap_done_callback(server_info);

    free(common_stub::cert->own_private_key_ptr);
    common_stub::cert->own_private_key_ptr = NULL;
    free(common_stub::cert->certificate_ptr[1]);
    common_stub::cert->certificate_ptr[1] = NULL;
    free(common_stub::cert->certificate_ptr[0]);
    common_stub::cert->certificate_ptr[0] = NULL;
    free(common_stub::cert);
    common_stub::cert = NULL;

    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_IPV6;
    server_info->omalw_server_security = CERTIFICATE;
    nsdl->bootstrap_done_callback(server_info);

    free(server_info->omalw_address_ptr->addr_ptr);
    server_info->omalw_address_ptr->addr_ptr = NULL;
    free(server_info->omalw_address_ptr);
    server_info->omalw_address_ptr = NULL;
    free(server_info);
    server_info = NULL;

    common_stub::clear();

}

void Test_M2MNsdlInterface::test_process_received_data()
{
    uint8_t *data = (uint8_t*)malloc(sizeof(uint8_t));
    uint16_t data_size = sizeof(uint16_t);
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));

    common_stub::int_value = 0;

    CHECK(nsdl->process_received_data(data,data_size,address) == true);

    common_stub::int_value = -1;

    CHECK(nsdl->process_received_data(data,data_size,address) == false);

    free(address);
    free(data);
    common_stub::clear();
}

void Test_M2MNsdlInterface::test_timer_expired()
{
    nsdl->timer_expired(M2MTimerObserver::NsdlExecution);
    CHECK(nsdl->_counter_for_nsdl == 1);

    if( nsdl->_endpoint == NULL){
        nsdl->_endpoint = (sn_nsdl_ep_parameters_s*)nsdl->memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(uint8_t));

    // For checking the registration update
    nsdl->timer_expired(M2MTimerObserver::Registration);

    nsdl->delete_endpoint();
    CHECK(nsdl->_endpoint == NULL);
}

void Test_M2MNsdlInterface::test_observation_to_be_sent()
{
    M2MResource *res = new M2MResource("name","name",
                                       M2MResourceInstance::INTEGER,
                                       M2MBase::Dynamic,false);

    uint8_t value[] = {"value"};
    m2mresourceinstance_stub::value = (uint8_t *)malloc(sizeof(value));
    memset( m2mresourceinstance_stub::value, 0, sizeof(value));
    memcpy(m2mresourceinstance_stub::value,value,sizeof(value));
    m2mresourceinstance_stub::int_value = sizeof(value);

    m2mbase_stub::uint16_value = 321;
    String *owned = new String("token");
    m2mbase_stub::string_value = owned;

    //CHECK if nothing crashes
    nsdl->observation_to_be_sent(res);

    delete owned;
    owned = NULL;

    free(m2mresourceinstance_stub::value);
    m2mresourceinstance_stub::value = NULL;

    m2mbase_stub::clear();
    m2mresourceinstance_stub::clear();

    delete res;
    res = NULL;
}

void Test_M2MNsdlInterface::test_resource_to_be_deleted()
{
    //Checking coverage for the code
    nsdl->resource_to_be_deleted("name");
}

void Test_M2MNsdlInterface::test_remove_object()
{
    String name = "name";
    M2MObject *obj = new M2MObject(name);
    nsdl->_object_list.push_back(obj);

    nsdl->remove_object((M2MBase*)obj);

    CHECK(nsdl->_object_list.empty() == true);

    nsdl->_object_list.clear();
    delete obj;
}

void Test_M2MNsdlInterface::test_add_object_to_list()
{
    M2MObject *obj = new M2MObject("name");
    nsdl->_object_list.push_back(obj);

    nsdl->add_object_to_list(obj);
    CHECK(nsdl->_object_list.size() == 1);

    nsdl->_object_list.clear();
    delete obj;
}
