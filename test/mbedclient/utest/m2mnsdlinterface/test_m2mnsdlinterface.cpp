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
#include "m2msecurity_stub.h"
#include "m2mnsdlobserver.h"
#include "m2mobject_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mresource.h"
#include "m2mbase_stub.h"
#include "m2mserver.h"
#include "m2msecurity.h"
#include "m2mtlvdeserializer_stub.h"
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

    void registration_error(uint8_t, bool retry = false){
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

    void bootstrap_wait(M2MSecurity *sec){
        if(sec) {
            boot_wait = true;
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
    bool boot_wait;
    bool boot_done;
    bool registered;
    bool register_updated;
    bool data_processed;
    bool unregistered;
    bool message_ready;
    bool value_update;
};

struct nsdl_s {
    uint16_t update_register_msg_id;
    uint16_t register_msg_len;
    uint16_t update_register_msg_len;

    uint16_t register_msg_id;
    uint16_t unregister_msg_id;

    uint16_t bootstrap_msg_id;
    uint16_t oma_bs_port;                                                       /* Bootstrap port */
    uint8_t oma_bs_address_len;                                                 /* Bootstrap address length */
    unsigned int sn_nsdl_endpoint_registered:1;
    bool handle_bootstrap_msg:1;

    struct grs_s *grs;
    uint8_t *oma_bs_address_ptr;                                                /* Bootstrap address pointer. If null, no bootstrap in use */
    sn_nsdl_ep_parameters_s *ep_information_ptr;                                // Endpoint parameters, Name, Domain etc..
    sn_nsdl_oma_server_info_t *nsp_address_ptr;                                 // NSP server address information

    void (*sn_nsdl_oma_bs_done_cb)(sn_nsdl_oma_server_info_t *server_info_ptr); /* Callback to inform application when bootstrap is done */
    void *(*sn_nsdl_alloc)(uint16_t);
    void (*sn_nsdl_free)(void *);
    uint8_t (*sn_nsdl_tx_callback)(struct nsdl_s *, sn_nsdl_capab_e , uint8_t *, uint16_t, sn_nsdl_addr_s *);
    uint8_t (*sn_nsdl_rx_callback)(struct nsdl_s *, sn_coap_hdr_s *, sn_nsdl_addr_s *);
    void (*sn_nsdl_oma_bs_done_cb_handle)(sn_nsdl_oma_server_info_t *server_info_ptr,
                                          struct nsdl_s *handle); /* Callback to inform application when bootstrap is done with nsdl handle */
};

Test_M2MNsdlInterface::Test_M2MNsdlInterface()
{
    observer = new TestObserver();
    nsdl = new M2MNsdlInterface(*observer);
    //nsdl->_server = new M2MServer();
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
    M2MObject *object = new M2MObject(*name, (char*)name->c_str());
    M2MObjectInstance* instance = new M2MObjectInstance(*object,*name, "", "name/0");

    M2MResource* create_resource = new M2MResource(*instance,
                                                   *name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   false,
                                                   "name");

    M2MResourceInstance* res_instance = new M2MResourceInstance(*create_resource, *name, *name,
                                       M2MResourceInstance::INTEGER, 0, (char*)name->c_str(), false);

    m2mobject_stub::instance_list.clear();
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.clear();
    m2mobjectinstance_stub::resource_list.push_back(create_resource);

    M2MObjectList list;
    list.push_back(object);

    m2mbase_stub::string_value = name->c_str();
    m2mbase_stub::mode_value = M2MBase::Static;


    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    m2mresource_stub::bool_value = true;
    m2mbase_stub::mode_value = M2MBase::Dynamic;

    m2mresource_stub::list.push_back(res_instance);
    m2mresourceinstance_stub::base_type = M2MBase::Resource;

    CHECK(nsdl->create_nsdl_list_structure(list)== true);

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
    common_stub::int_value = 0;
    M2MObject *object = new M2MObject("name", "name");

    CHECK(nsdl->remove_nsdl_resource(object) == false);

    common_stub::int_value = 1;

    CHECK(nsdl->remove_nsdl_resource(object) == true);
    delete object;
}

void Test_M2MNsdlInterface::test_create_bootstrap_resource()
{
    common_stub::uint_value = 11;
    CHECK(nsdl->create_bootstrap_resource(NULL, "") == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->create_bootstrap_resource(NULL, "") == false);
}

void Test_M2MNsdlInterface::test_send_register_message()
{
    common_stub::uint_value = 12;
    common_stub::int_value = 0;
    CHECK(nsdl->send_register_message(NULL,4,100,SN_NSDL_ADDRESS_TYPE_IPV6) == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->send_register_message(NULL,4,100,SN_NSDL_ADDRESS_TYPE_IPV6) == false);
}

void Test_M2MNsdlInterface::test_send_update_registration()
{
    common_stub::uint_value = 23;
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(1));
    CHECK(nsdl->send_update_registration(120) == true);

    // Update lifetime value
    common_stub::uint_value = 1;
    CHECK(nsdl->send_update_registration(100) == true);

    // Lifetime value is 0, don't change the existing lifetime value
    common_stub::uint_value = 1;
    CHECK(nsdl->send_update_registration(0) == true);

    free(nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_send_unregister_message()
{
    common_stub::uint_value = 22;
    CHECK(nsdl->send_unregister_message() == true);

    // Unreg already in progress
    common_stub::uint_value = 0;
    CHECK(nsdl->send_unregister_message() == true);
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
    nsdl_s* handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(handle,0,sizeof(nsdl_s));

    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;


    coap_header->options_list_ptr = (sn_coap_options_list_s *)malloc(sizeof(sn_coap_options_list_s));
    memset(coap_header->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

    coap_header->options_list_ptr->max_age = 2;

    coap_header->options_list_ptr->location_path_len = 2;
    coap_header->options_list_ptr->location_path_ptr = (uint8_t *)malloc(sizeof(coap_header->options_list_ptr->location_path_len));
    memset(coap_header->options_list_ptr->location_path_ptr, 0, sizeof(coap_header->options_list_ptr->location_path_len));

    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
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
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

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

    nsdl->received_from_server_callback(handle,coap_header,NULL);
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
    nsdl->received_from_server_callback(handle,coap_header,NULL);
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

    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    observer->data_processed = false;
    observer->registered = false;
    observer->unregistered = false;
    observer->register_error = false;

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_OPTION;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_INCOMPLETE;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_PRECONDITION_FAILED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_UNAUTHORIZED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_GATEWAY;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);


    coap_header->msg_id = 8;
    handle->unregister_msg_id = 8;

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_DELETED;
    observer->register_error = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->unregistered == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;

    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_GATEWAY_TIMEOUT;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    // Receive initial bs message with error
    handle->unregister_msg_id = 0;
    observer->boot_error = false;
    nsdl->_bootstrap_id = 8;
    handle->bootstrap_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->boot_error == true);

    //_update_id == msg_id
    handle->update_register_msg_id = 10;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_updated == true);

    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    coap_header->coap_status = COAP_STATUS_OK;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    coap_header->msg_id = 11;
    CHECK( 0== nsdl->received_from_server_callback(handle,coap_header,NULL) );

    handle->update_register_msg_id = 0;
    handle->register_msg_id = 0;
    handle->unregister_msg_id = 0;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    uint8_t object[] = {"name"};

    coap_header->uri_path_ptr = object;
    coap_header->uri_path_len = sizeof(object);

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    //Test methods that are not allowed
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    //Continue testing with post method
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    uint8_t object_instance[] = {"name/0"};

    coap_header->uri_path_ptr = object_instance;
    coap_header->uri_path_len = sizeof(object_instance);

    coap_header->payload_ptr = (uint8_t*)malloc(1);
    m2mobjectinstance_stub::bool_value = true;
    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    M2MObject *obj = new M2MObject("name", "name");

    m2mbase_stub::string_value = "name";
    m2mbase_stub::nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*) malloc(sizeof(sn_nsdl_static_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters->path = (uint8_t*)malloc(5);
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[0] = 'n';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[1] = 'a';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[2] = 'm';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[3] = 'e';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[4] = '\0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->pathlen = 5;

    nsdl->_object_list.push_back(obj);

    m2mobject_stub::inst = new M2MObjectInstance(*obj, "name", "", "");

    m2mobject_stub::header = (sn_coap_hdr_s*) malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobject_stub::header,0,sizeof(sn_coap_hdr_s));

    m2mobjectinstance_stub::header =  (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobjectinstance_stub::header, 0, sizeof(sn_coap_hdr_s));
    m2mobjectinstance_stub::header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    common_stub::coap_header = NULL;
    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;

    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobjectinstance_stub::header = NULL;

    m2mobjectinstance_stub::header =  (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobjectinstance_stub::header, 0, sizeof(sn_coap_hdr_s));

    m2mobjectinstance_stub::header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobjectinstance_stub::header = NULL;

    free(coap_header->payload_ptr);
    coap_header->payload_ptr = NULL;

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;

    free(coap_header->payload_ptr);
    free(m2mobject_stub::header);
    m2mobject_stub::header = NULL;

    uint8_t object_instance1[] = {"name/65536"};

    coap_header->uri_path_ptr = object_instance1;
    coap_header->uri_path_len = sizeof(object_instance1);

    obj = new M2MObject("name", "name");

    nsdl->_object_list.push_back(obj);

    m2mobject_stub::inst = new M2MObjectInstance(*obj, "name", "", "");

    m2mobject_stub::header = (sn_coap_hdr_s*) malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobject_stub::header,0,sizeof(sn_coap_hdr_s));

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;

    free(m2mobject_stub::header);

    uint8_t resource[] = {"name/0/resource"};

    coap_header->uri_path_ptr = resource;
    coap_header->uri_path_len = sizeof(resource);

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    // Test EMPTY ACK
    coap_header->msg_code = COAP_MSG_CODE_EMPTY;
    obj = new M2MObject("0", "0");
    m2mbase_stub::string_value = "0";
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, "name", "", "");

    uint8_t *token = (uint8_t*) malloc(4);
    token[0] = 't';
    token[1] = 'o';
    token[2] = 'k';
    token[3] = 'e';
    coap_header->token_ptr = token;
    coap_header->token_len = 4;

    M2MResource res2(*m2mobject_stub::inst, "test","test",M2MResourceInstance::STRING,false, "test");
    m2mbase_stub::object_token = token;
    m2mbase_stub::object_token_len = 4;
    m2mobject_stub::base_type = M2MBase::Resource;
    res2.set_observation_token(token, 4);

    m2mobjectinstance_stub::resource_list.push_back(&res2);
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));

    // Test RESET message
    coap_header->msg_type = COAP_MSG_TYPE_RESET;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobject_stub::base_type = M2MBase::Object;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobject_stub::base_type = M2MBase::ResourceInstance;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    free(token);
    delete m2mobject_stub::inst;
    delete obj;
    nsdl->_object_list.clear();


    // Bootstrap cases start from here
    // handle_bootstrap_put_message() invalid object name
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address, 0, sizeof(sn_nsdl_addr_s));
    address->addr_len = 1;
    address->addr_ptr = (uint8_t *)malloc(1);
    address->addr_ptr[0] = 1;
    address->port = 5683;
    handle->oma_bs_address_len = 1;
    handle->oma_bs_port = 5683;
    handle->oma_bs_address_ptr = (uint8_t *)malloc(1);
    handle->oma_bs_address_ptr[0] = 1;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    observer->boot_error = false;
    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);

    // handle_bootstrap_put_message() invalid content type
    obj = new M2MObject("0", "0");
    m2mbase_stub::string_value = "0";
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, "name", "", "");
    uint8_t security[] = {"0"};
    coap_header->uri_path_ptr = security;
    coap_header->uri_path_len = sizeof(security);
    M2MResource res(*m2mobject_stub::inst,"test","test",M2MResourceInstance::STRING,false,"test");
    m2mobjectinstance_stub::resource_list.push_back(&res);
    observer->boot_error = false;
    m2msecurity_stub::resource = new M2MResource(*m2mobject_stub::inst,"1","type",M2MResourceInstance::STRING,false,"1");
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);

    // handle_bootstrap_put_message() success
    coap_header->token_ptr = String::convert_integer_to_array(1,coap_header->token_len);
    observer->boot_error = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    coap_header->content_format = sn_coap_content_format_e(99);
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);

    // handle_bootstrap_put_message() TLV parsing fails
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = false;
    m2mtlvdeserializer_stub::bool_value = true;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::NotAllowed;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;

    // handle_bootstrap_put_message() TLV object instance
    obj = new M2MObject("1", "1");
    m2mbase_stub::string_value = "1";
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, "name","", "");
    uint8_t server[] = {"1"};
    coap_header->uri_path_ptr = server;
    coap_header->uri_path_len = 1;

    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = false;
    m2mtlvdeserializer_stub::bool_value = true;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);

    // handle_bootstrap_put_message() TLV server object
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    m2mtlvdeserializer_stub::bool_value = false;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);

    // handle_bootstrap_put_message() TLV not resource
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = false;
    m2mtlvdeserializer_stub::bool_value = false;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    // handle_bootstrap_delete() object name not match
    observer->boot_error = false;
    nsdl->_bootstrap_id = 8;
    handle->bootstrap_msg_id = 8;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    coap_header->msg_id = 8;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    coap_header->msg_id = 18;
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);

    free(common_stub::coap_header);
    common_stub::coap_header = NULL;

    // handle_bootstrap_delete() _identity_accepted false
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    free(common_stub::coap_header);
    common_stub::coap_header = NULL;

    // handle_bootstrap_delete() object name not match
    observer->boot_error = false;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    coap_header->msg_id = 8;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    coap_header->msg_id = 18;
    uint8_t object_name[] = {"0/0"};
    coap_header->uri_path_ptr = object_name;
    coap_header->uri_path_len = 3;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    free(common_stub::coap_header);
    common_stub::coap_header = NULL;

    // handle_bootstrap_delete() object name not match
    observer->boot_error = false;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    coap_header->msg_id = 8;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    coap_header->msg_id = 18;
    uint8_t invalid[] = {"0/0/1"};
    coap_header->uri_path_ptr = invalid;
    coap_header->uri_path_len = 5;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    //handle_bootstrap_finished() path does not match
    coap_header->uri_path_ptr = server;
    coap_header->uri_path_len = 1;
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    //handle_bootstrap_finished() send coap response
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    m2msecurity_stub::string_value = new String("coaps://");
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    //handle_bootstrap_finished() success no security
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::NoSecurity;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = false;
    observer->boot_error = false;
    observer->boot_wait = false;
    observer->boot_done = false;
    coap_header->uri_path_ptr = (uint8_t*)malloc(2);
    coap_header->uri_path_len = 2;
    coap_header->uri_path_ptr[0] = 'b';
    coap_header->uri_path_ptr[1] = 's';
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);
    CHECK(observer->boot_wait == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() success certificate
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2mresourceinstance_stub::int_value = 10;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = false;
    observer->boot_error = false;
    observer->boot_wait = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);
    CHECK(observer->boot_wait == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, Psk not supported
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Psk;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = false;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, Bootstrap server
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = true;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, key size 0
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2msecurity_stub::int_value = false;
    m2msecurity_stub::bool_value = false;
    m2mresourceinstance_stub::int_value = 0;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, _security null
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2msecurity_stub::int_value = false;
    m2msecurity_stub::bool_value = false;
    m2mresourceinstance_stub::int_value = 0;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    delete m2mobject_stub::inst;
    //delete m2mbase_stub::string_value;
    //m2mbase_stub::string_value = NULL;
    nsdl->_object_list.clear();
    m2mobjectinstance_stub::resource_list.clear();
    delete obj;
    delete m2msecurity_stub::string_value;
    delete m2msecurity_stub::resource;

    free(m2mbase_stub::nsdl_resource->static_resource_parameters->path);
    free(m2mbase_stub::nsdl_resource->static_resource_parameters);
    free(m2mbase_stub::nsdl_resource);

    free(common_stub::coap_header);
    free(address->addr_ptr);
    free(address);
    free(coap_header->token_ptr);
    free(coap_header->uri_path_ptr);
    free(coap_header);
    free(handle->oma_bs_address_ptr);
    free(handle);
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

    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";
    M2MObject *object = new M2MObject("name", "name");
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;
    m2mbase_stub::nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*) malloc(sizeof(sn_nsdl_static_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters->path = (uint8_t*)malloc(5);
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[0] = 'n';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[1] = 'a';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[2] = 'm';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[3] = 'e';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[4] = '\0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->pathlen = 5;

    nsdl->_object_list.push_back(object);

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    m2mobject_stub::bool_value = true;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);
    m2mobject_stub::base_type = M2MBase::ResourceInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    delete object;
    free(m2mbase_stub::nsdl_resource->static_resource_parameters->path);
    free(m2mbase_stub::nsdl_resource->static_resource_parameters);
    free(m2mbase_stub::nsdl_resource);

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

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object,"name","", "");

    M2MResource* create_resource = new M2MResource(*instance,
                                                   "res",
                                                   "res",
                                                   M2MResourceInstance::INTEGER,
                                                   false,
                                                   "res");
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
    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::base_type = M2MBase::Resource;
    m2mbase_stub::base_type = M2MBase::Resource;
    m2mobject_stub::bool_value = true;
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
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    free(address);

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

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";
    m2mbase_stub::bool_value = false;
    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object,"name","","");
    M2MResource* create_resource = new M2MResource(*instance,
                                                   "name",
                                                   "name",
                                                   M2MResourceInstance::INTEGER,
                                                   false,
                                                   "name");
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

    m2mbase_stub::nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*) malloc(sizeof(sn_nsdl_static_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters->path = (uint8_t*)malloc(5);
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[0] = 'n';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[1] = 'a';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[2] = 'm';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[3] = 'e';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[4] = '\0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->pathlen = 5;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));
    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
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
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    free(address);
    free(m2mbase_stub::nsdl_resource->static_resource_parameters->path);
    free(m2mbase_stub::nsdl_resource->static_resource_parameters);
    free(m2mbase_stub::nsdl_resource);

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
    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    common_stub::int_value = 0;

    m2mbase_stub::nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    m2mbase_stub::nsdl_resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*) malloc(sizeof(sn_nsdl_static_resource_parameters_s));

    m2mbase_stub::nsdl_resource->static_resource_parameters->path = (uint8_t*)malloc(7);
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[0] = 'n';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[1] = 'a';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[2] = 'm';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[3] = 'e';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[4] = '/';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[5] = '0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[6] = '\0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->pathlen = 7;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name/0";
    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object,"name", "", "name/0");
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;
    m2mobject_stub::bool_value = true;
    m2mobject_stub::instance_list.push_back(instance);
    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::DELETE_ALLOWED;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::base_type = M2MBase::Object;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    delete instance;
    delete object;
    free(m2mbase_stub::nsdl_resource->static_resource_parameters->path);
    free(m2mbase_stub::nsdl_resource->static_resource_parameters);
    free(m2mbase_stub::nsdl_resource);
    free(common_stub::coap_header);
    free(address);
    free(coap_header);
    m2mobject_stub::instance_list.clear();
}

/*
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
}
*/
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
    m2mbase_stub::clear();
    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, "name", "", "");
    M2MResource *res = new M2MResource(*instance,"res", "res", M2MResourceInstance::INTEGER,false, "res");
    M2MResource *res2 = new M2MResource(*instance,"res2", "res2", M2MResourceInstance::INTEGER,false, "res2");


    M2MResourceInstance* res_instance = new M2MResourceInstance(*res,
                                                                "res", "res",
                                                                M2MResourceInstance::INTEGER,0,"",false);
    M2MResourceInstance* res_instance_1 = new M2MResourceInstance(*res2,
                                                                  "res2", "res2",
                                                                  M2MResourceInstance::INTEGER,0,"",false);
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
    m2mbase_stub::string_value = "token";

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
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(res2, 1, instance_list_ids);
    free(common_stub::coap_header);

    m2mresourceinstance_stub::resource_type = M2MResource::OPAQUE;

    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(res2, 1, instance_list_ids);
    free(common_stub::coap_header);

    m2mresource_stub::list.clear();
    m2mresource_stub::int_value = 0;

    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(res, 500, instance_list_ids);
    free(common_stub::coap_header);

    M2MObjectInstance *object_instance = new M2MObjectInstance(*object, "name", "","");
    m2mobject_stub::int_value = 1;
    m2mobject_stub::base_type = M2MBase::Object;
    m2mobject_stub::inst = object_instance;
    m2mobjectinstance_stub::resource_list.push_back(res);
    nsdl->_object_list.push_back(object);
    instance_list_ids.push_back(1);
    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(object, 1, instance_list_ids);
    free(common_stub::coap_header);

    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(object, 500, instance_list_ids, true);
    free(common_stub::coap_header);

    //CHECK if nothing crashes
    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(object_instance, 1, instance_list_ids);
    free(common_stub::coap_header);

    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    nsdl->observation_to_be_sent(object_instance, 500, instance_list_ids);
    free(common_stub::coap_header);

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
    M2MObject *object = new M2MObject("name", "name");
    nsdl->resource_to_be_deleted(object);
    delete object;
}

void Test_M2MNsdlInterface::test_value_updated()
{
    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance *object_instance = new M2MObjectInstance(*object, "name", "", "");
    M2MResource *resource = new M2MResource(*object_instance,
                                            "resource_name",
                                            "resource_type",
                                            M2MResourceInstance::INTEGER,
                                            false,
                                            "resource_name");

    M2MResourceInstance *resource_instance = new M2MResourceInstance(*resource,
                                                                     "resource_name",
                                                                     "resource_type",
                                                                     M2MResourceInstance::INTEGER,
                                                                     0,"",false);

    m2mobject_stub::base_type = M2MBase::Object;
    m2mbase_stub::string_value = "name";
    m2mbase_stub::operation = M2MBase::GET_ALLOWED;
    nsdl->value_updated(object,"name");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;

    nsdl->value_updated(object_instance,"name/0");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    common_stub::resource = (sn_nsdl_dynamic_resource_parameters_s*)malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    memset(common_stub::resource,0, sizeof(sn_nsdl_dynamic_resource_parameters_s));

    common_stub::resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*)malloc(sizeof(sn_nsdl_static_resource_parameters_s));
    memset(common_stub::resource->static_resource_parameters,0, sizeof(sn_nsdl_static_resource_parameters_s));

    common_stub::resource->static_resource_parameters->resource = (uint8_t*)malloc(2);
    memset(common_stub::resource->static_resource_parameters->resource,0, 2);

    common_stub::resource->static_resource_parameters->mode = SN_GRS_STATIC;
    m2mbase_stub::mode_value = M2MBase::Static;

    common_stub::resource->observable = false;
    m2mbase_stub::bool_value = true;

    m2mresourceinstance_stub::int_value = 2;
    uint8_t value[] = "1";
    m2mresourceinstance_stub::value = value;

    m2mresourceinstance_stub::base_type = M2MBase::Resource;

    nsdl->value_updated(resource,"name/0/name");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mresourceinstance_stub::clear();
    free(common_stub::resource->static_resource_parameters->resource);
    free(common_stub::resource->static_resource_parameters);
    free(common_stub::resource);
    common_stub::resource = NULL;
    common_stub::clear();

    m2mresourceinstance_stub::base_type = M2MBase::ResourceInstance;

    common_stub::resource = (sn_nsdl_dynamic_resource_parameters_s*)malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    memset(common_stub::resource,0, sizeof(sn_nsdl_dynamic_resource_parameters_s));

    common_stub::resource->static_resource_parameters =
            (sn_nsdl_static_resource_parameters_s*)malloc(sizeof(sn_nsdl_static_resource_parameters_s));
    memset(common_stub::resource->static_resource_parameters,0, sizeof(sn_nsdl_static_resource_parameters_s));

    common_stub::resource->static_resource_parameters->resource = (uint8_t*)malloc(2);
    memset(common_stub::resource->static_resource_parameters->resource,0, 2);

    common_stub::resource->static_resource_parameters->mode = SN_GRS_STATIC;
    m2mbase_stub::mode_value = M2MBase::Static;

    m2mresourceinstance_stub::int_value = 2;
    m2mresourceinstance_stub::value = value;

    nsdl->value_updated(resource_instance,"name/0/name/0");
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mbase_stub::is_value_updated_function_set = true;
    nsdl->value_updated(resource_instance,"name/0/name/0");
    CHECK(observer->value_update == false);
    observer->value_update = false;

    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();

    m2mresourceinstance_stub::clear();

    free(common_stub::resource->static_resource_parameters->resource);
    free(common_stub::resource->static_resource_parameters);
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
    m2mbase_stub::string_value = "name";
    m2mbase_stub::object_instance_name = "name/0";
    m2mbase_stub::resource_name_inst = "name/0/resource_name/0";
    m2mbase_stub::resource_name = "name/0/resource_name";
    m2mbase_stub::find_resource = true;
    uint8_t *token = (uint8_t*) malloc(4);
    token[0] = 't';
    token[1] = 'o';
    token[2] = 'k';
    token[3] = 'e';
    m2mbase_stub::object_token = token;
    m2mbase_stub::object_token_len = 4;

    uint8_t *inst_token = (uint8_t*) malloc(4);
    inst_token[0] = 't';
    inst_token[1] = 'o';
    inst_token[2] = 'k';
    inst_token[3] = 'k';
    m2mbase_stub::object_inst_token = inst_token;
    m2mbase_stub::object_inst_token_len = 4;

    uint8_t *resource_token = (uint8_t*) malloc(4);
    resource_token[0] = 't';
    resource_token[1] = 'k';
    resource_token[2] = 'k';
    resource_token[3] = 'k';
    m2mbase_stub::resource_token = resource_token;
    m2mbase_stub::resource_token_len = 4;

    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance *object_instance = new M2MObjectInstance(*object,"name/0", "","name/0");
    M2MResource *resource = new M2MResource(*object_instance,
                                            "resource_name",
                                            "resource_type",
                                            M2MResourceInstance::INTEGER,
                                            false,
                                            "resource_name");

    M2MResourceInstance *resource_instance = new M2MResourceInstance(*resource,
                                                                     "0",
                                                                     "resource_type",
                                                                     M2MResourceInstance::INTEGER,0,"0",false);

    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;
    m2mobject_stub::instance_list.push_back(object_instance);
    m2mobjectinstance_stub::resource_list.push_back(resource);
    m2mresource_stub::list.push_back(resource_instance);
    m2mobject_stub::base_type = M2MBase::Object;
    nsdl->_object_list.push_back(object);
    m2mresource_stub::bool_value = true;
    CHECK(nsdl->find_resource("name") == object);
    CHECK(nsdl->find_resource("name1") == NULL);
    CHECK(nsdl->find_resource("name", token, 4) == object);
    CHECK(nsdl->find_resource("name", token, 5) == NULL);

    m2mbase_stub::ret_counter = 0;
    CHECK(nsdl->find_resource("name/0") == object_instance);
    m2mbase_stub::ret_counter = 0;
    CHECK(nsdl->find_resource("name/0", inst_token, 4) == object_instance);
    m2mbase_stub::ret_counter = 0;

    CHECK(nsdl->find_resource("name/0/resource_name") == resource);
    m2mbase_stub::ret_counter = 0;
    CHECK(nsdl->find_resource("name/0/resource_name", resource_token, 4) == resource);

    m2mbase_stub::ret_counter = 0;
    CHECK(nsdl->find_resource("name/0/resource_name/0") == resource_instance);

    free(token);
    free(inst_token);
    free(resource_token);

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
    M2MObject *obj = new M2MObject("name", "name");
    nsdl->_object_list.push_back(obj);

    nsdl->remove_object((M2MBase*)obj);

    CHECK(nsdl->_object_list.empty() == true);

    nsdl->_object_list.clear();
    delete obj;
}

void Test_M2MNsdlInterface::test_add_object_to_list()
{
    M2MObject *obj = new M2MObject("name", "name");
    nsdl->_object_list.push_back(obj);

    nsdl->add_object_to_list(obj);
    CHECK(nsdl->_object_list.size() == 1);

    nsdl->_object_list.clear();
    delete obj;
}

void Test_M2MNsdlInterface::test_send_delayed_response()
{
    common_stub::int_value = 0;
    m2mbase_stub::int_value = 0;

    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, "name", "", "");
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(nsdl->_nsdl_handle,0,sizeof(nsdl_s));

    sn_nsdl_oma_server_info_t * nsp_address = (sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    memset(nsp_address,0,sizeof(sn_nsdl_oma_server_info_t));
    sn_nsdl_addr_s* address = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    M2MResource* resource = new M2MResource(*instance,
                                            "name",
                                            "name",
                                            M2MResourceInstance::INTEGER,
                                            false,
                                            "name");

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
    delete instance;
    delete resource;
    free(m2mresource_stub::delayed_token);
    m2mresource_stub::delayed_token = NULL;
    m2mresource_stub::delayed_token_len = 0;
}

void Test_M2MNsdlInterface::test_get_nsdl_handle()
{
    CHECK(nsdl->get_nsdl_handle() == nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_endpoint_name()
{
    String endpoint = "test";
    nsdl->_endpoint_name = endpoint;
    CHECK(nsdl->endpoint_name() == endpoint);
}
