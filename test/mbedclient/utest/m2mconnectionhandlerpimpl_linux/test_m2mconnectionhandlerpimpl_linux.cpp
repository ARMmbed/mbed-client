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
#include "test_m2mconnectionhandlerpimpl_linux.h"
#include "mbed-client-linux/m2mconnectionhandlerpimpl.h"
#include "m2mconnectionobserver.h"
#include "m2msecurity.h"
#include "common_stub.h"
#include "m2mconnectionsecurity_stub.h"

class M2MConnection_TestObserver : public M2MConnectionObserver {

public:
    M2MConnection_TestObserver():
    obj(NULL){}

    virtual ~M2MConnection_TestObserver(){}

    void set_class_object(M2MConnectionHandlerPimpl *impl) {obj = impl;}
    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){
        dataAvailable = true;
        if(obj) {
            obj->_receive_data = false;
        }

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
    M2MConnectionHandlerPimpl *obj;
};

Test_M2MConnectionHandlerPimpl_linux::Test_M2MConnectionHandlerPimpl_linux()
{
    observer = new M2MConnection_TestObserver();
    m2mconnectionsecurityimpl_stub::clear();
    common_stub::int_value = 0;
    common_stub::thread = 14;
    handler = new M2MConnectionHandlerPimpl(NULL, *observer, NULL , M2MInterface::NOT_SET, M2MInterface::Uninitialized);

}

Test_M2MConnectionHandlerPimpl_linux::~Test_M2MConnectionHandlerPimpl_linux()
{
    handler->_listen_thread = 1;
    delete handler;
    delete observer;
}

void Test_M2MConnectionHandlerPimpl_linux::test_bind_connection()
{
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::int_value = 5;

    CHECK( handler->bind_connection(7) == true);
}

void Test_M2MConnectionHandlerPimpl_linux::test_resolve_server_address()
{
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    M2MSecurity* sec = new M2MSecurity(M2MSecurity::M2MServer);

    M2MConnectionHandlerPimpl* tcp_handler = new M2MConnectionHandlerPimpl(NULL, *observer, NULL , M2MInterface::TCP, M2MInterface::Uninitialized);
    CHECK(tcp_handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, sec) == false);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;
    delete tcp_handler;

    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, sec) == false);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;

    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET6;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    handler->_socket_server = -1;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, sec) == false);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);

    tcp_handler = new M2MConnectionHandlerPimpl(NULL, *observer, NULL , M2MInterface::TCP, M2MInterface::Uninitialized);
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET6;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    tcp_handler->_socket_server = -1;
    CHECK(tcp_handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, sec) == false);
    delete tcp_handler;

    M2MConnectionSecurity* conSec = new M2MConnectionSecurity(M2MConnectionSecurity::TLS);
    handler->_security_impl = conSec;
    m2mconnectionsecurityimpl_stub::int_value = 0;

    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, sec) == true);
    handler->_security_impl = NULL;
    delete conSec;

    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;

    free(handler->_received_packet_address);
    handler->_received_packet_address = NULL;
//    CHECK(handler->_receive_data == true);
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, sec) == false);

    delete sec;
}

void Test_M2MConnectionHandlerPimpl_linux::test_send_data()
{
    sn_nsdl_addr_s* addr = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(addr, 0, sizeof(sn_nsdl_addr_s));
    uint8_t* data = (uint8_t*)malloc(5);
    CHECK( false == handler->send_data(data, 0 , NULL));

    common_stub::int_value = 4;
    CHECK(true == handler->send_data(data, 0 , addr));

    M2MConnectionHandlerPimpl* tcp_handler = new M2MConnectionHandlerPimpl(NULL, *observer, NULL , M2MInterface::TCP, M2MInterface::Uninitialized);
    common_stub::int_value = 4;
    CHECK(true == tcp_handler->send_data(data, 0 , addr));
    delete tcp_handler;

    common_stub::int_value = -1;
    CHECK(false == handler->send_data(data, 0 , addr));


    M2MConnectionSecurity* conSec = new M2MConnectionSecurity(M2MConnectionSecurity::TLS);
    handler->_security_impl = conSec;
    handler->_use_secure_connection = true;
    m2mconnectionsecurityimpl_stub::int_value = 0;
    CHECK(false == handler->send_data(data, 0 , addr));
    m2mconnectionsecurityimpl_stub::int_value = 5;
    CHECK(true == handler->send_data(data, 0 , addr));
    handler->_security_impl = NULL;
    delete conSec;

    free(data);
    free(addr);
}

void Test_M2MConnectionHandlerPimpl_linux::test_start_listening_for_data()
{
    handler->start_listening_for_data();
}

void Test_M2MConnectionHandlerPimpl_linux::test_data_receive()
{
    handler->data_receive(NULL);

    M2MConnectionHandlerPimpl *obj = new M2MConnectionHandlerPimpl(NULL,
                                                                   *observer,
                                                                   NULL,
                                                                   M2MInterface::TCP_QUEUE,
                                                                   M2MInterface::Uninitialized);

    handler->_receive_data = true;
    observer->error = false;
    common_stub::int_value = -1;
    handler->data_receive(obj);
    CHECK(observer->error == true);
    CHECK(handler->_receive_data == false);

    M2MConnectionHandlerPimpl* tcp_handler = new M2MConnectionHandlerPimpl(NULL, *observer, NULL , M2MInterface::TCP, M2MInterface::Uninitialized);
    tcp_handler->_receive_data = true;
    observer->error = false;
    common_stub::int_value = 2;
    tcp_handler->data_receive(obj);
    CHECK(observer->error == true);
    CHECK(tcp_handler->_receive_data == false);
    delete tcp_handler;

    tcp_handler = new M2MConnectionHandlerPimpl(NULL, *observer, NULL , M2MInterface::TCP, M2MInterface::Uninitialized);
    tcp_handler->_receive_data = true;
    observer->error = false;
    observer->set_class_object(tcp_handler);
    common_stub::int_value = 6;
    tcp_handler->data_receive(obj);
    CHECK(observer->error == false);
    CHECK(tcp_handler->_receive_data == false);
    delete tcp_handler;

    observer->set_class_object(handler);
    handler->_receive_data = true;
    observer->dataAvailable = false;
    common_stub::int_value = 6;
    handler->data_receive(obj);
    CHECK(observer->dataAvailable == true);

    M2MConnectionSecurity* conSec = new M2MConnectionSecurity(M2MConnectionSecurity::TLS);
    handler->_security_impl = conSec;
    handler->_use_secure_connection = true;
    handler->_receive_data = true;
    m2mconnectionsecurityimpl_stub::int_value = -1;
    handler->data_receive(obj);
    CHECK(handler->_receive_data == false);

    observer->dataAvailable = false;
    handler->_receive_data = true;
    m2mconnectionsecurityimpl_stub::use_inc_int = true;
    m2mconnectionsecurityimpl_stub::inc_int_value = 0;
    handler->data_receive(obj);
    CHECK(handler->_receive_data == false);
    CHECK(observer->dataAvailable == true);
    handler->_security_impl = NULL;
    delete conSec;

    delete obj;
    obj = NULL;
}

void Test_M2MConnectionHandlerPimpl_linux::test_stop_listening()
{
    handler->_receive_data == true;
    handler->stop_listening();
    CHECK(handler->_receive_data == false);
}

void Test_M2MConnectionHandlerPimpl_linux::test_sendToSocket()
{
    const char buf[] = "hello";
    handler->sendToSocket((unsigned char *)&buf, 5);
}

void Test_M2MConnectionHandlerPimpl_linux::test_receiveFromSocket()
{
    unsigned char *buf = (unsigned char *)malloc(6);
    handler->receiveFromSocket(buf, 5);
    free(buf);
}
