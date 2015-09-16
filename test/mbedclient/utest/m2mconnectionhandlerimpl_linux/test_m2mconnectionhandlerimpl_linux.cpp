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
#include "test_m2mconnectionhandlerimpl_linux.h"
#include "m2mconnectionhandlerpimpl_stub.h"
#include "m2mconnectionobserver.h"
#include "common_stub.h"

class M2MConnection_TestObserver : public M2MConnectionObserver {

public:
    M2MConnection_TestObserver(){}

    virtual ~M2MConnection_TestObserver(){}

    void set_class_object(M2MConnectionHandler *impl) {}
    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){
        dataAvailable = true;
    }

    void socket_error(uint8_t error_code){error = true;}

    void address_ready(const M2MConnectionObserver::SocketAddress &,
                       M2MConnectionObserver::ServerType,
                       const uint16_t){addressReady = true;}

    void data_sent(){dataSent = true;}

    bool dataAvailable;
    bool error;
    bool addressReady;
    bool dataSent;
};

Test_M2MConnectionHandler_linux::Test_M2MConnectionHandler_linux()
{
    observer = new M2MConnection_TestObserver();
    common_stub::int_value = 0;
    common_stub::thread = 14;
    handler = new M2MConnectionHandler(*observer, NULL ,M2MInterface::NOT_SET,M2MInterface::Uninitialized);
}

Test_M2MConnectionHandler_linux::~Test_M2MConnectionHandler_linux()
{
//    handler->_listen_thread = 1;
    delete handler;
    delete observer;
}

void Test_M2MConnectionHandler_linux::test_bind_connection()
{
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::int_value = 5;

    m2mconnectionhandlerpimpl_stub::bool_value = true;
    CHECK( handler->bind_connection(7) == true);
}

void Test_M2MConnectionHandler_linux::test_resolve_server_address()
{
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    m2mconnectionhandlerpimpl_stub::bool_value = true;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, NULL) == true);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;

    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET6;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
//    handler->_socket_server = -1;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, NULL) == true);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;

//    free(handler->_received_packet_address);
//    handler->_received_packet_address = NULL;
    m2mconnectionhandlerpimpl_stub::bool_value = false;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, NULL) == false);
}

void Test_M2MConnectionHandler_linux::test_send_data()
{
    sn_nsdl_addr_s* addr = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(addr, 0, sizeof(sn_nsdl_addr_s));
    m2mconnectionhandlerpimpl_stub::bool_value = false;
    CHECK( false == handler->send_data(NULL, 0 , NULL));

    common_stub::int_value = 4;
    m2mconnectionhandlerpimpl_stub::bool_value = true;
    CHECK(true == handler->send_data(NULL, 0 , addr));

    common_stub::int_value = -1;
    m2mconnectionhandlerpimpl_stub::bool_value = false;
    CHECK(false == handler->send_data(NULL, 0 , addr));
    free(addr);
}

void Test_M2MConnectionHandler_linux::test_start_listening_for_data()
{
    handler->start_listening_for_data();
}

//void Test_M2MConnectionHandler_linux::test_data_receive()
//{
//    handler->data_receive(NULL);

//    M2MConnectionHandler *obj = new M2MConnectionHandler(*observer, NULL,M2MInterface::Uninitialized);

//    handler->_receive_data = true;
//    observer->error = false;
//    common_stub::int_value = -1;
//    handler->data_receive(obj);
//    CHECK(observer->error == true);
//    CHECK(handler->_receive_data == false);

//    handler->_receive_data = true;
//    observer->set_class_object(handler);
//    observer->dataAvailable = false;
//    common_stub::int_value = 6;
//    handler->data_receive(obj);
//    CHECK(observer->dataAvailable == true);

//    delete obj;
//    obj = NULL;
//}

void Test_M2MConnectionHandler_linux::test_stop_listening()
{
//    handler->_receive_data == true;
    handler->stop_listening();
//    CHECK(handler->_receive_data == false);
}

void Test_M2MConnectionHandler_linux::test_sendToSocket()
{
    const char buf[] = "hello";
    handler->sendToSocket((unsigned char *)&buf, 5);
}

void Test_M2MConnectionHandler_linux::test_receiveFromSocket()
{
    unsigned char *buf = (unsigned char *)malloc(6);
    handler->receiveFromSocket(buf, 5);
    free(buf);
}
