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
#include "test_m2mconnectionhandlerpimpl_mbed.h"
#include "m2mconnectionobserver.h"
#include "common_stub.h"
#include "m2mconnectionsecurity_stub.h"
#include "m2msecurity_stub.h"
#include "m2mconnectionhandler.h"

class TestObserver : public M2MConnectionObserver {

public:
    TestObserver(){}

    virtual ~TestObserver(){}

    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){dataAvailable = true;}

    void socket_error(uint8_t ){ error = true; }

    void address_ready(const M2MConnectionObserver::SocketAddress &,
                       M2MConnectionObserver::ServerType,
                       const uint16_t){}

    void data_sent(){}

    bool dataAvailable;
    bool error;
    bool addressReady;
    bool dataSent;
};

Test_M2MConnectionHandlerPimpl_mbed::Test_M2MConnectionHandlerPimpl_mbed()
{
    observer = new TestObserver();
    m2mconnectionsecurityimpl_stub::clear();
    common_stub::clear();
    handler = new M2MConnectionHandlerPimpl(NULL,*observer,NULL,M2MInterface::NOT_SET,M2MInterface::Uninitialized);
}

Test_M2MConnectionHandlerPimpl_mbed::~Test_M2MConnectionHandlerPimpl_mbed()
{
    delete handler;
    delete observer;
}

void Test_M2MConnectionHandlerPimpl_mbed::test_constructor()
{
    TestObserver obs;
    M2MConnectionHandlerPimpl impl = M2MConnectionHandlerPimpl(NULL,obs,NULL,M2MInterface::TCP_QUEUE,M2MInterface::Uninitialized);
    CHECK(impl._socket_stack == SOCKET_STACK_UNINIT);

    M2MConnectionHandlerPimpl impl2 = M2MConnectionHandlerPimpl(NULL,obs,NULL,M2MInterface::NOT_SET,M2MInterface::LwIP_IPv4);
    CHECK(impl2._socket_stack == SOCKET_STACK_LWIP_IPV4);

    M2MConnectionHandlerPimpl impl3 = M2MConnectionHandlerPimpl(NULL,obs,NULL,M2MInterface::NOT_SET,M2MInterface::LwIP_IPv6);
    CHECK(impl3._socket_stack == SOCKET_STACK_LWIP_IPV6);

    M2MConnectionHandlerPimpl impl4 = M2MConnectionHandlerPimpl(NULL,obs,NULL,M2MInterface::NOT_SET,M2MInterface::Reserved);
    CHECK(impl4._socket_stack == SOCKET_STACK_RESERVED);

    M2MConnectionHandlerPimpl impl5 = M2MConnectionHandlerPimpl(NULL,obs,NULL,M2MInterface::NOT_SET,M2MInterface::Nanostack_IPv6);
    CHECK(impl5._socket_stack == SOCKET_STACK_NANOSTACK_IPV6);

    M2MConnectionHandlerPimpl impl6 = M2MConnectionHandlerPimpl(NULL,obs,NULL,M2MInterface::NOT_SET,M2MInterface::Unknown);
    CHECK(impl6._socket_stack == SOCKET_STACK_MAX);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_bind_connection()
{
    common_stub::error = SOCKET_ERROR_NONE;
    handler->_network_stack = M2MInterface::LwIP_IPv4;
    CHECK( handler->bind_connection(7) == true);

    handler->_network_stack = M2MInterface::Nanostack_IPv6;
    CHECK( handler->bind_connection(7) == true);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_resolve_server_address()
{
    common_stub::error = SOCKET_ERROR_NONE;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer, NULL) == true);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_send_data()
{
    sn_nsdl_addr_s* addr = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(addr, 0, sizeof(sn_nsdl_addr_s));
    CHECK( false == handler->send_data(NULL, 0 , NULL));

    uint8_t* data = (uint8_t*)malloc(5);
    CHECK(true == handler->send_data(data, 0 , addr));

    handler->_binding_mode = M2MInterface::TCP;
    CHECK(true == handler->send_data(data, 0 , addr));

    M2MConnectionSecurity* conSec = new M2MConnectionSecurity(M2MConnectionSecurity::TLS);
    handler->_security_impl = conSec;
    handler->_use_secure_connection = true;
    CHECK(false == handler->send_data(data, 0 , addr));
    m2mconnectionsecurityimpl_stub::int_value = 5;
    CHECK(true == handler->send_data(data, 0 , addr));
    handler->_security_impl = NULL;
    delete conSec;

    free(data);
    free(addr);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_start_listening_for_data()
{
    CHECK(true == handler->start_listening_for_data());
}

void Test_M2MConnectionHandlerPimpl_mbed::test_send_handler()
{
    handler->send_handler(NULL, 10);
    CHECK(observer->dataSent == true);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_receive_handler()
{
    handler->_network_stack = M2MInterface::LwIP_IPv4;
    handler->receive_handler(NULL);
    CHECK(observer->dataAvailable == true);

    observer->dataAvailable = false;
    handler->_network_stack = M2MInterface::Nanostack_IPv6;
    handler->receive_handler(NULL);
    CHECK(observer->dataAvailable == true);

    handler->receive_handler(NULL);
    CHECK(observer->error == true);

    common_stub::error = SOCKET_ERROR_BAD_FAMILY;
    handler->receive_handler(NULL);
    CHECK(observer->error == true);

    common_stub::error = SOCKET_ERROR_NONE;

    M2MConnectionSecurity* conSec = new M2MConnectionSecurity(M2MConnectionSecurity::TLS);
    handler->_security_impl = conSec;
    handler->_use_secure_connection = true;
    m2mconnectionsecurityimpl_stub::int_value = -5;
    handler->receive_handler(NULL);
    CHECK(observer->error == true);

    handler->_binding_mode = M2MInterface::TCP;
    observer->dataAvailable = false;
    m2mconnectionsecurityimpl_stub::int_value = 5;
    handler->receive_handler(NULL);
    CHECK(observer->dataAvailable == true);

    observer->error = false;
    m2mconnectionsecurityimpl_stub::int_value = 2;
    handler->receive_handler(NULL);
    CHECK(observer->error == true);

    handler->_security_impl = NULL;
    delete conSec;
}

void Test_M2MConnectionHandlerPimpl_mbed::test_receive_handshake_handler()
{
    handler->_is_handshaking = true;
    m2mconnectionsecurityimpl_stub::int_value = M2MConnectionHandler::CONNECTION_ERROR_WANTS_READ;
    handler->receive_handshake_handler(NULL);
    CHECK(true == handler->_is_handshaking);

    m2mconnectionsecurityimpl_stub::int_value = 0;
    handler->receive_handshake_handler(NULL);
    CHECK(false == handler->_is_handshaking);

    handler->_is_handshaking = true;
    m2mconnectionsecurityimpl_stub::int_value = -10;
    handler->receive_handshake_handler(NULL);
    CHECK(false == handler->_is_handshaking);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_dns_handler()
{
    common_stub::event = (socket_event_t*)malloc(sizeof(socket_event_t));
    memset(common_stub::event, 0, sizeof(socket_event_t));
    handler->_network_stack = M2MInterface::LwIP_IPv4;
    common_stub::bool_value = true;

    socket_addr sa;
    handler->dns_handler(NULL,sa,NULL);
    CHECK(observer->addressReady == true);

    common_stub::bool_value = false;
    handler->_network_stack = M2MInterface::Nanostack_IPv6;
    handler->dns_handler(NULL,sa,NULL);
    CHECK(observer->addressReady == true);

    handler->dns_handler(NULL,sa,NULL);
    CHECK(observer->error == true);

    M2MConnectionSecurity* conSec = new M2MConnectionSecurity(M2MConnectionSecurity::TLS);
    handler->_security_impl = conSec;
    handler->_use_secure_connection = true;
    m2mconnectionsecurityimpl_stub::int_value = -5;
    m2msecurity_stub::int_value = M2MSecurity::Psk;

    M2MSecurity* sec = new M2MSecurity(M2MSecurity::M2MServer);
    handler->_security = sec;
    handler->dns_handler(NULL,sa,NULL);
    CHECK(false == handler->_is_handshaking);
    m2mconnectionsecurityimpl_stub::int_value = 5;
    handler->dns_handler(NULL,sa,NULL);
    CHECK(true == handler->_is_handshaking);

    handler->_security_impl = NULL;
    delete conSec;
    delete sec;
    free(common_stub::event);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_error_handler()
{
    handler->error_handler(NULL,SOCKET_ERROR_BAD_FAMILY);
    CHECK(observer->error == true);
}

void Test_M2MConnectionHandlerPimpl_mbed::test_stop_listening()
{
    // Empty function to cover the cases
    handler->stop_listening();
}

void Test_M2MConnectionHandlerPimpl_mbed::test_sendToSocket()
{
    const char buf[] = "hello";
    CHECK( 5 == handler->sendToSocket((unsigned char *)&buf, 5) );

    common_stub::error = SOCKET_ERROR_WOULD_BLOCK;
    CHECK( M2MConnectionHandler::CONNECTION_ERROR_WANTS_WRITE == handler->sendToSocket((unsigned char *)&buf, 5) );

    common_stub::error = SOCKET_ERROR_ALREADY_CONNECTED;
    CHECK( -1 == handler->sendToSocket((unsigned char *)&buf, 5) );
}

void Test_M2MConnectionHandlerPimpl_mbed::test_receiveFromSocket()
{
    unsigned char *buf = (unsigned char *)malloc(6);
    CHECK( 5 == handler->receiveFromSocket(buf, 5));

    common_stub::error = SOCKET_ERROR_WOULD_BLOCK;
    CHECK( M2MConnectionHandler::CONNECTION_ERROR_WANTS_READ == handler->receiveFromSocket(buf, 5) );

    common_stub::error = SOCKET_ERROR_ALREADY_CONNECTED;
    CHECK( -1 == handler->receiveFromSocket(buf, 5) );

    free(buf);
}
