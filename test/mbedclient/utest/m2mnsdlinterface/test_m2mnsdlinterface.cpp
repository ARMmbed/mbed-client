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
        value_update = true;
    }

    bool register_error;
    bool boot_error;
    bool boot_done;
    bool registered;
    bool register_updated;
    bool data_processed;
    bool unregistered;
    bool message_ready;
    bool value_update;
};

struct nsdl_s {
    struct grs_s *grs;

    uint8_t *oma_bs_address_ptr;                                                /* Bootstrap address pointer. If null, no bootstrap in use */
    uint8_t oma_bs_address_len;                                                 /* Bootstrap address length */
    uint16_t oma_bs_port;                                                       /* Bootstrap port */
    void (*sn_nsdl_oma_bs_done_cb)(sn_nsdl_oma_server_info_t *server_info_ptr); /* Callback to inform application when bootstrap is done */

    sn_nsdl_ep_parameters_s *ep_information_ptr;    // Endpoint parameters, Name, Domain etc..
    sn_nsdl_oma_server_info_t *nsp_address_ptr;     // NSP server address information
    uint8_t sn_nsdl_endpoint_registered;

    uint16_t register_msg_id;
    uint16_t unregister_msg_id;

    void *(*sn_nsdl_alloc)(uint16_t);
    void (*sn_nsdl_free)(void *);
    uint8_t (*sn_nsdl_tx_callback)(struct nsdl_s *, sn_nsdl_capab_e , uint8_t *, uint16_t, sn_nsdl_addr_s *);
    uint8_t (*sn_nsdl_rx_callback)(struct nsdl_s *, sn_coap_hdr_s *, sn_nsdl_addr_s *);
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
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);

    M2MResource* create_resource = new M2MResource(*instance,
                                                   *name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,
                                                   false);

    M2MResourceInstance* res_instance = new M2MResourceInstance(*name,
                                                                *name,
                                                                M2MResourceInstance::INTEGER,
                                                                *instance);

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

    m2mresource_stub::list.push_back(res_instance);

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

    m2mresource_stub::list.clear();
    delete res_instance;
    res_instance = NULL;
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

    nsdl->_update_id = 0;
    common_stub::uint_value = 1;
    CHECK(nsdl->send_update_registration(0) == true);

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

    coap_header->options_list_ptr->location_path_len = 2;
    coap_header->options_list_ptr->location_path_ptr = (uint8_t *)malloc(sizeof(coap_header->options_list_ptr->location_path_len));
    memset(coap_header->options_list_ptr->location_path_ptr, 0, sizeof(coap_header->options_list_ptr->location_path_len));

    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    free(nsdl->_endpoint->location_ptr);
    nsdl->_endpoint->location_ptr = NULL;

    uint8_t life1[] = {"120"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(life1));
    memcpy(nsdl->_endpoint->lifetime_ptr,life1,sizeof(life1));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(life1);

    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(NULL,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(coap_header->options_list_ptr->max_age_ptr);
    coap_header->options_list_ptr->max_age_ptr = NULL;

    free(coap_header->options_list_ptr->location_path_ptr);
    coap_header->options_list_ptr->location_path_ptr = NULL;

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

    //msg_id is unique
    nsdl->_update_id = 0;
    nsdl->_register_id = 0;
    nsdl->_unregister_id = 0;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    uint8_t object[] = {"name"};

    coap_header->uri_path_ptr = object;
    coap_header->uri_path_len = sizeof(object);

    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));

    uint8_t object_instance[] = {"name/0"};

    coap_header->uri_path_ptr = object_instance;
    coap_header->uri_path_len = sizeof(object_instance);

    coap_header->payload_ptr = (uint8_t*)malloc(1);
    m2mobjectinstance_stub::bool_value = true;
    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));

    M2MObject *obj = new M2MObject("name");

    m2mbase_stub::string_value = new String("name");

    nsdl->_object_list.push_back(obj);

    m2mobject_stub::inst = new M2MObjectInstance("name",*obj);

    m2mobject_stub::header = (sn_coap_hdr_s*) malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobject_stub::header,0,sizeof(sn_coap_hdr_s));

    m2mobjectinstance_stub::header =  (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobjectinstance_stub::header, 0, sizeof(sn_coap_hdr_s));
    m2mobjectinstance_stub::header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;

    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));

    m2mobjectinstance_stub::header = NULL;

    m2mobjectinstance_stub::header =  (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobjectinstance_stub::header, 0, sizeof(sn_coap_hdr_s));

    m2mobjectinstance_stub::header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;
    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));


    free(coap_header->payload_ptr);
    coap_header->payload_ptr = NULL;

    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));

    delete m2mobject_stub::inst;
    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;
    nsdl->_object_list.clear();
    delete obj;

    free(coap_header->payload_ptr);
    free(m2mobject_stub::header);
    m2mobject_stub::header = NULL;

    uint8_t object_instance1[] = {"name/65536"};

    coap_header->uri_path_ptr = object_instance1;
    coap_header->uri_path_len = sizeof(object_instance1);

    obj = new M2MObject("name");

    m2mbase_stub::string_value = new String("name");

    nsdl->_object_list.push_back(obj);

    m2mobject_stub::inst = new M2MObjectInstance("name",*obj);

    m2mobject_stub::header = (sn_coap_hdr_s*) malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobject_stub::header,0,sizeof(sn_coap_hdr_s));

    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));

    delete m2mobject_stub::inst;
    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;
    nsdl->_object_list.clear();
    delete obj;

    free(m2mobject_stub::header);

    uint8_t resource[] = {"name/0/resource"};

    coap_header->uri_path_ptr = resource;
    coap_header->uri_path_len = sizeof(resource);

    CHECK(0== nsdl->received_from_server_callback(NULL,coap_header,NULL));

    free(coap_header);
}

void Test_M2MNsdlInterface::test_resource_callback()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    common_stub::coap_header->payload_ptr =(uint8_t*)malloc(1);
    common_stub::coap_header->payload_len = 1;

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    common_stub::int_value = 0;

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    common_stub::int_value = 0;
    m2mbase_stub::string_value = new String("name");
    M2MObject *object = new M2MObject("name");
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);


    delete object;
    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;

    free(common_stub::coap_header);
    free(address);
    free(coap_header);
}

void Test_M2MNsdlInterface::test_resource_callback_get()
{
}

void Test_M2MNsdlInterface::test_resource_callback_put()
{
    uint8_t value[] = {"name/0/resource"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    String *name = new String("name");
    String *res_name = new String("res");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;

    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    M2MResource* create_resource = new M2MResource(*instance,
                                                   *res_name,
                                                   *res_name,
                                                   M2MResourceInstance::INTEGER,
                                                   false);
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
    m2mobject_stub::bool_value = true;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);
    m2mobject_stub::bool_value = false;

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
    delete res_name;
    name = NULL;

    m2mbase_stub::clear();
    common_stub::clear();
    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();
}

void Test_M2MNsdlInterface::test_resource_callback_post()
{
    uint8_t value[] = {"name/0/name"};
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
    m2mbase_stub::bool_value = false;
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    M2MResource* create_resource = new M2MResource(*instance,
                                                   *name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,false);
    m2mobject_stub::int_value = 2;
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.push_back(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::POST_ALLOWED;

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

    m2mobject_stub::base_type = M2MBase::ResourceInstance;
    m2mbase_stub::base_type = M2MBase::ResourceInstance;
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

void Test_M2MNsdlInterface::test_resource_callback_delete()
{
    uint8_t value[] = {"name/0"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;

    common_stub::int_value = 0;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = new String("name");
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;
    m2mobject_stub::bool_value = true;
    m2mobject_stub::instance_list.push_back(instance);

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::DELETE_ALLOWED;
    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobjectinstance_stub::base_type = M2MBase::Object;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    delete instance;
    delete object;
    delete name;
    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;

    free(common_stub::coap_header);
    free(address);
    free(coap_header);
    m2mobject_stub::instance_list.clear();
}

void Test_M2MNsdlInterface::test_resource_callback_reset()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_type = COAP_MSG_TYPE_RESET;
    m2mobjectinstance_stub::base_type = M2MBase::Object;
    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    M2MResource* create_resource = new M2MResource(*instance,
                                                   *name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,false);
    m2mobject_stub::int_value = 2;
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.push_back(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);
    // No response for RESET message
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);


    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    // No response for RESET message
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);

    m2mobject_stub::base_type = M2MBase::Resource;
    // No response for RESET message
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);

    delete instance;
    delete object;
    delete name;
    delete create_resource;
    free(address);
    free(coap_header);
    m2mobject_stub::instance_list.clear();
    m2mobjectinstance_stub::resource_list.clear();
    //nsdl->_object_list.clear();
    /*uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));
    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));
    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);
    coap_header->msg_type = COAP_MSG_TYPE_RESET;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = new String("name");
    M2MObject *object = new M2MObject(*name);    
    nsdl->_object_list.push_back(object);
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 0);

    uint8_t value2[] = {"name/0"};
    coap_header->uri_path_ptr = value2;
    coap_header->uri_path_len = sizeof(value2);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;
    m2mobject_stub::bool_value = true;
    m2mobject_stub::instance_list.push_back(instance);
    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 0);

    uint8_t value3[] = {"name/0/1"};
    coap_header->uri_path_ptr = value3;
    coap_header->uri_path_len = sizeof(value3);
    M2MResource *res = new M2MResource(*instance,
                                       "1","1",
                                       M2MResourceInstance::INTEGER,
                                       false);
    nsdl->_object_list.clear();
    nsdl->_object_list.push_back(object);
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;
    m2mobject_stub::bool_value = true;
    m2mobjectinstance_stub::resource_list.push_back(res);
    m2mobjectinstance_stub::int_value = 0;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 0);

    delete instance;
    delete object;
    delete name;
    delete res;
    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;

    free(common_stub::coap_header);
    free(address);
    free(coap_header);*/
}

void Test_M2MNsdlInterface::test_bootstrap_done_callback()
{
    const uint8_t server_address[] = {10, 45, 3, 85};
    sn_nsdl_oma_server_info_t *server_info =(sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    server_info->omalw_address_ptr = (sn_nsdl_addr_ *)malloc(sizeof(sn_nsdl_addr_));
    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_IPV4;
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
    const uint8_t hostname_address[] = {"abc.xyz.sss.sdsd"};
    server_info =(sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    server_info->omalw_address_ptr = (sn_nsdl_addr_ *)malloc(sizeof(sn_nsdl_addr_));
    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_HOSTNAME;
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
    // FD00:FF1:CE0B:A5E1:1068:AF13:9B61:D557
    const uint8_t ipv6_address[16] = {0xFD, 0x00, 0x0F, 0xF1,
                                      0xCE, 0x0B, 0xA5, 0xE1,
                                      0x10, 0x68, 0xAF, 0x13,
                                      0x9B, 0x61, 0xD5, 0x57};

    server_info =(sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    server_info->omalw_address_ptr = (sn_nsdl_addr_ *)malloc(sizeof(sn_nsdl_addr_));
    server_info->omalw_address_ptr->type = SN_NSDL_ADDRESS_TYPE_IPV6;
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

void Test_M2MNsdlInterface::test_stop_timers()
{
    // Check if there is no memory leak or crash
    nsdl->stop_timers();
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
    Vector<uint16_t> instance_list_ids;
    M2MObject *object = new M2MObject("name");
    M2MObjectInstance* instance = new M2MObjectInstance("name",*object);
    M2MResource *res = new M2MResource(*instance,
                                       "name","name",
                                       M2MResourceInstance::INTEGER,
                                       false,true);

    M2MResource *res2 = new M2MResource(*instance,
                                           "res2","res2",
                                           M2MResourceInstance::INTEGER,
                                           false,true);

    M2MResourceInstance* res_instance = new M2MResourceInstance("res2","res2",
                                                                M2MResourceInstance::INTEGER,
                                                                *instance);
    M2MResourceInstance* res_instance_1 = new M2MResourceInstance("res2","res2",
                                                                M2MResourceInstance::INTEGER,
                                                                *instance);
    m2mresource_stub::list.clear();
    m2mresource_stub::list.push_back(res_instance);
    m2mresource_stub::list.push_back(res_instance_1);
    m2mresource_stub::int_value = 2;
    instance_list_ids.push_back(0);

    uint8_t value[] = {"value"};
    m2mresourceinstance_stub::value = (uint8_t *)malloc(sizeof(value));
    memset( m2mresourceinstance_stub::value, 0, sizeof(value));
    memcpy(m2mresourceinstance_stub::value,value,sizeof(value));
    m2mresourceinstance_stub::int_value = sizeof(value);

    m2mbase_stub::uint16_value = 321;
    String *owned = new String("token");
    m2mbase_stub::string_value = owned;

    m2mresourceinstance_stub::base_type = M2MBase::Resource;

    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(nsdl->_nsdl_handle,0,sizeof(nsdl_s));
    sn_nsdl_oma_server_info_t * nsp_address = (sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    memset(nsp_address,0,sizeof(sn_nsdl_oma_server_info_t));
    sn_nsdl_addr_s* address = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    nsdl->_nsdl_handle->nsp_address_ptr = nsp_address;
    memset(nsdl->_nsdl_handle->nsp_address_ptr,0,sizeof(sn_nsdl_oma_server_info_t));
    nsdl->_nsdl_handle->nsp_address_ptr->omalw_address_ptr = address;

    //CHECK if nothing crashes
    nsdl->observation_to_be_sent(res2, 1, instance_list_ids);

    m2mresourceinstance_stub::resource_type = M2MResource::OPAQUE;

    //CHECK if nothing crashes
    nsdl->observation_to_be_sent(res2, 1, instance_list_ids);

    m2mresource_stub::list.clear();
    m2mresource_stub::int_value = 0;

    //CHECK if nothing crashes
    nsdl->observation_to_be_sent(res, 500, instance_list_ids);

    M2MObjectInstance *object_instance = new M2MObjectInstance("name",*object);
    m2mobject_stub::int_value = 1;    
    m2mobject_stub::base_type = M2MBase::Object;
    m2mobject_stub::inst = object_instance;
    m2mobjectinstance_stub::resource_list.push_back(res);
    nsdl->_object_list.push_back(object);
    instance_list_ids.push_back(1);
    //CHECK if nothing crashes
    nsdl->observation_to_be_sent(object, 1, instance_list_ids);
    nsdl->observation_to_be_sent(object, 500, instance_list_ids, true);

    //CHECK if nothing crashes
    nsdl->observation_to_be_sent(object_instance, 1, instance_list_ids);
    nsdl->observation_to_be_sent(object_instance, 500, instance_list_ids);

    delete owned;
    owned = NULL;

    free(m2mresourceinstance_stub::value);
    m2mresourceinstance_stub::value = NULL;

    m2mbase_stub::clear();
    m2mresourceinstance_stub::clear();
    m2mobjectinstance_stub::clear();
    m2mobject_stub::clear();
    m2mresource_stub::list.clear();
    delete res;
    res = NULL;

    delete res2;
    res2 = NULL;

    delete res_instance;
    res_instance = NULL;

    delete res_instance_1;
    res_instance_1 = NULL;


    delete object_instance;
    object_instance = NULL;

    delete object;
    object = NULL;

    delete instance;
    instance = NULL;

    free(nsp_address);
    free(address);
    free(nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_resource_to_be_deleted()
{
    //Checking coverage for the code
    nsdl->resource_to_be_deleted("name");
}

void Test_M2MNsdlInterface::test_value_updated()
{
    M2MObject *object = new M2MObject("name");
    M2MObjectInstance *object_instance = new M2MObjectInstance("name",*object);
    M2MResource *resource = new M2MResource(*object_instance,
                                            "resource_name",
                                            "resource_type",
                                            M2MResourceInstance::INTEGER,
                                            false);

    M2MResourceInstance *resource_instance = new M2MResourceInstance("resource_name",
                                                                     "resource_type",
                                                                     M2MResourceInstance::INTEGER,
                                                                     *object_instance);


    m2mobject_stub::base_type = M2MBase::Object;
    m2mbase_stub::string_value = new String("name");
    m2mbase_stub::operation = M2MBase::GET_ALLOWED;

    nsdl->value_updated(object,"name");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;

    nsdl->value_updated(object_instance,"name/0");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    common_stub::resource = (sn_nsdl_resource_info_s*)malloc(sizeof(sn_nsdl_resource_info_s));
    memset(common_stub::resource,0, sizeof(sn_nsdl_resource_info_s));
    common_stub::resource->resource = (uint8_t*)malloc(2);
    memset(common_stub::resource->resource,0, 2);

    common_stub::resource->mode = SN_GRS_STATIC;
    m2mbase_stub::mode_value = M2MBase::Static;

    m2mresourceinstance_stub::int_value = 2;
    uint8_t value[] = "1";
    m2mresourceinstance_stub::value = value;

    m2mresourceinstance_stub::base_type = M2MBase::Resource;

    nsdl->value_updated(resource,"name/0/name");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mresourceinstance_stub::clear();
    free(common_stub::resource);
    common_stub::resource = NULL;
    common_stub::clear();

    m2mresourceinstance_stub::base_type = M2MBase::ResourceInstance;

    common_stub::resource = (sn_nsdl_resource_info_s*)malloc(sizeof(sn_nsdl_resource_info_s));
    memset(common_stub::resource,0, sizeof(sn_nsdl_resource_info_s));
    common_stub::resource->resource = (uint8_t*)malloc(2);
    memset(common_stub::resource->resource,0, 2);

    common_stub::resource->mode = SN_GRS_STATIC;
    m2mbase_stub::mode_value = M2MBase::Static;

    m2mresourceinstance_stub::int_value = 2;
    m2mresourceinstance_stub::value = value;

    nsdl->value_updated(resource_instance,"name/0/name/0");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;

    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();

    m2mresourceinstance_stub::clear();

    free(common_stub::resource);
    common_stub::resource = NULL;
    common_stub::clear();

    delete resource_instance;
    delete resource;
    delete object_instance;
    delete object;
}

void Test_M2MNsdlInterface::test_find_resource()
{
    m2mbase_stub::string_value = new String("name");
    M2MObject *object = new M2MObject("name");
    M2MObjectInstance *object_instance = new M2MObjectInstance("name",*object);
    M2MResource *resource = new M2MResource(*object_instance,
                                            "resource_name",
                                            "resource_type",
                                            M2MResourceInstance::INTEGER,
                                            false);

    M2MResourceInstance *resource_instance = new M2MResourceInstance("name",
                                                                     "resource_type",
                                                                      M2MResourceInstance::INTEGER,
                                                                     *object_instance);


    m2mobject_stub::instance_list.push_back(object_instance);
    m2mobjectinstance_stub::resource_list.push_back(resource);
    m2mresource_stub::list.push_back(resource_instance);

    m2mobject_stub::base_type = M2MBase::Object;

    nsdl->_object_list.push_back(object);

    CHECK(nsdl->find_resource("name") != NULL);

    m2mbase_stub::int_value  = 0;

    CHECK(nsdl->find_resource("name/0") != NULL);

    CHECK(nsdl->find_resource("name/0/name") != NULL);

    m2mresource_stub::bool_value = true;

    CHECK(nsdl->find_resource("name/0/name/0") != NULL);

    CHECK(nsdl->find_resource("name1") == NULL);
    CHECK(nsdl->find_resource("name/1") == NULL);
    CHECK(nsdl->find_resource("name/0/name1") == NULL);
    CHECK(nsdl->find_resource("name/0/name/1") == NULL);

    delete m2mbase_stub::string_value;
    m2mbase_stub::string_value = NULL;

    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();

    m2mresourceinstance_stub::clear();
    m2mresource_stub::clear();
    m2mobjectinstance_stub::clear();
    m2mobject_stub::clear();

    delete resource_instance;
    delete resource;
    delete object_instance;
    delete object;
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

void Test_M2MNsdlInterface::test_send_delayed_response()
{
    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::int_value = 0;

    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(nsdl->_nsdl_handle,0,sizeof(nsdl_s));

    sn_nsdl_oma_server_info_t * nsp_address = (sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    memset(nsp_address,0,sizeof(sn_nsdl_oma_server_info_t));
    sn_nsdl_addr_s* address = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    M2MResource* resource = new M2MResource(*instance,
                                            *name,
                                            *name,
                                            M2MResourceInstance::INTEGER,
                                            M2MResource::Dynamic,
                                            false);

    uint8_t val[] = {"name"};
    m2mresource_stub::delayed_token = (uint8_t*)malloc(sizeof(val));
    memcpy(m2mresource_stub::delayed_token,val,sizeof(val));
    m2mresource_stub::delayed_token_len = sizeof(val);

    m2mresourceinstance_stub::base_type = M2MBase::Resource;

    nsdl->_nsdl_handle->nsp_address_ptr = nsp_address;
    memset(nsdl->_nsdl_handle->nsp_address_ptr,0,sizeof(sn_nsdl_oma_server_info_t));
    nsdl->_nsdl_handle->nsp_address_ptr->omalw_address_ptr = address;

    nsdl->send_delayed_response(resource);

    free(nsp_address);
    free(address);
    free(nsdl->_nsdl_handle);

    delete object;
    delete name;
    name = NULL;
    delete instance;
    instance = NULL;
    delete resource;
    resource = NULL;
    free(m2mresource_stub::delayed_token);
    m2mresource_stub::delayed_token = NULL;
    m2mresource_stub::delayed_token_len = 0;
}
