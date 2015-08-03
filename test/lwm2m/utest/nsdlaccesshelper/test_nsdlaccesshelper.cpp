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
#include "test_nsdlaccesshelper.h"
#include "common_stub.h"
#include "m2mnsdlinterface_stub.h"
#include "m2mnsdlobserver.h"

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
    bool data_processed;
    bool unregistered;
    bool message_ready;

};

Test_NsdlAccessHelper::Test_NsdlAccessHelper()
{    
    observer = new TestObserver();
}

Test_NsdlAccessHelper::~Test_NsdlAccessHelper()
{
    delete observer;
    observer = NULL;
}

void Test_NsdlAccessHelper::test_nsdl_c_callback()
{

    CHECK(__nsdl_c_callback(NULL,NULL,NULL,SN_NSDL_PROTOCOL_HTTP) == 0 );

    m2mnsdlinterface_stub::int_value = 1;
    __nsdl_interface = new M2MNsdlInterface(*observer);

    CHECK(__nsdl_c_callback(NULL,NULL,NULL,SN_NSDL_PROTOCOL_HTTP) == 1 );

    delete __nsdl_interface;
    __nsdl_interface = NULL;
}

void Test_NsdlAccessHelper::test_nsdl_c_memory_alloc()
{
    void *ptr = __nsdl_c_memory_alloc(6);
    CHECK(ptr == NULL);

    __nsdl_interface = new M2MNsdlInterface(*observer);
    ptr = __nsdl_c_memory_alloc(6);
    CHECK(ptr != NULL);

    delete __nsdl_interface;
    __nsdl_interface = NULL;
    free(ptr);
}

void Test_NsdlAccessHelper::test_nsdl_c_memory_free()
{
    void* ptr = malloc(7);
    __nsdl_c_memory_free(ptr);

    CHECK(ptr != NULL);

    __nsdl_interface = new M2MNsdlInterface(*observer);
    __nsdl_c_memory_free(ptr);

    delete __nsdl_interface;
    __nsdl_interface = NULL;
    ptr = NULL;
    //No need to check anything, since memory leak is the test
}

void Test_NsdlAccessHelper::test_nsdl_c_send_to_server()
{
    CHECK(__nsdl_c_send_to_server(NULL, SN_NSDL_PROTOCOL_HTTP, NULL, 0, NULL) == 0);

    m2mnsdlinterface_stub::int_value = 1;
    __nsdl_interface = new M2MNsdlInterface(*observer);
    CHECK(__nsdl_c_send_to_server(NULL, SN_NSDL_PROTOCOL_HTTP, NULL, 0, NULL) == 1);

    delete __nsdl_interface;
    __nsdl_interface = NULL;
}

void Test_NsdlAccessHelper::test_nsdl_c_received_from_server()
{
    CHECK( 0 == __nsdl_c_received_from_server(NULL, NULL, NULL));

    m2mnsdlinterface_stub::int_value = 1;
    __nsdl_interface = new M2MNsdlInterface(*observer);
    CHECK( 1 == __nsdl_c_received_from_server(NULL, NULL, NULL));

    delete __nsdl_interface;
    __nsdl_interface = NULL;
}

void Test_NsdlAccessHelper::test_nsdl_c_bootstrap_done()
{
    __nsdl_c_bootstrap_done(NULL);

    m2mnsdlinterface_stub::int_value = 1;
    __nsdl_interface = new M2MNsdlInterface(*observer);
    __nsdl_c_bootstrap_done(NULL);

    delete __nsdl_interface;
    __nsdl_interface = NULL;
}

void Test_NsdlAccessHelper::test_socket_malloc()
{
    void *ptr = __socket_malloc(NULL, 6);
    CHECK(ptr != NULL);

    free(ptr);
}

void Test_NsdlAccessHelper::test_socket_free()
{
    void* ptr = malloc(7);
    __socket_free(NULL, ptr);

    ptr = NULL;
    //No need to check anything, since memory leak is the test
}


