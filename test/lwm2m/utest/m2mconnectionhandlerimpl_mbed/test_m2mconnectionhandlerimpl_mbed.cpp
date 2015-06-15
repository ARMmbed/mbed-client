/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerimpl_mbed.h"
#include "m2mconnectionobserver.h"
#include "common_stub.h"

class TestObserver : public M2MConnectionObserver {

public:
    TestObserver(){}

    virtual ~TestObserver(){}

    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){}

    void socket_error(uint8_t error_code){}

    void address_ready(const M2MConnectionObserver::SocketAddress &,
                       M2MConnectionObserver::ServerType,
                       const uint16_t){}

    void data_sent(){}

    bool dataAvailable;
    bool error;
    bool addressReady;
    bool dataSent;
};

Test_M2MConnectionHandlerImpl_mbed::Test_M2MConnectionHandlerImpl_mbed()
{
    observer = new TestObserver();
    handler = new M2MConnectionHandlerImpl(*observer,M2MInterface::Uninitialized);
}

Test_M2MConnectionHandlerImpl_mbed::~Test_M2MConnectionHandlerImpl_mbed()
{
    delete handler;
    delete observer;
}

void Test_M2MConnectionHandlerImpl_mbed::test_constructor()
{
    TestObserver obs;
    M2MConnectionHandlerImpl impl = M2MConnectionHandlerImpl(obs,M2MInterface::Uninitialized);
    CHECK(impl._socket_stack == SOCKET_STACK_UNINIT);

    M2MConnectionHandlerImpl impl2 = M2MConnectionHandlerImpl(obs,M2MInterface::LwIP_IPv4);
    CHECK(impl2._socket_stack == SOCKET_STACK_LWIP_IPV4);

    M2MConnectionHandlerImpl impl3 = M2MConnectionHandlerImpl(obs,M2MInterface::LwIP_IPv6);
    CHECK(impl3._socket_stack == SOCKET_STACK_LWIP_IPV6);

    M2MConnectionHandlerImpl impl4 = M2MConnectionHandlerImpl(obs,M2MInterface::Reserved);
    CHECK(impl4._socket_stack == SOCKET_STACK_RESERVED);

    M2MConnectionHandlerImpl impl5 = M2MConnectionHandlerImpl(obs,M2MInterface::Nanostack_IPv6);
    CHECK(impl5._socket_stack == SOCKET_STACK_NANOSTACK_IPV6);

    M2MConnectionHandlerImpl impl6 = M2MConnectionHandlerImpl(obs,M2MInterface::Unknown);
    CHECK(impl6._socket_stack == SOCKET_STACK_MAX);
}

void Test_M2MConnectionHandlerImpl_mbed::test_bind_connection()
{
    common_stub::error = SOCKET_ERROR_NONE;
    handler->_network_stack = M2MInterface::LwIP_IPv4;
    CHECK( handler->bind_connection(7) == true);

    handler->_network_stack = M2MInterface::Nanostack_IPv6;
    CHECK( handler->bind_connection(7) == true);
}

void Test_M2MConnectionHandlerImpl_mbed::test_resolve_server_address()
{
    common_stub::error = SOCKET_ERROR_NONE;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer) == true);
}

void Test_M2MConnectionHandlerImpl_mbed::test_send_data()
{
    sn_nsdl_addr_s* addr = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(addr, 0, sizeof(sn_nsdl_addr_s));
    CHECK( false == handler->send_data(NULL, 0 , NULL));

    CHECK(true == handler->send_data(NULL, 0 , addr));
    free(addr);
}

void Test_M2MConnectionHandlerImpl_mbed::test_listen_for_data()
{
    handler->listen_for_data();
}

void Test_M2MConnectionHandlerImpl_mbed::test_send_handler()
{
    handler->send_handler(SOCKET_ERROR_NONE);
    CHECK(observer->dataSent == true);

    handler->send_handler(SOCKET_ERROR_BAD_FAMILY);
    CHECK(observer->error == true);
}

void Test_M2MConnectionHandlerImpl_mbed::test_receive_handler()
{
    handler->_network_stack = M2MInterface::LwIP_IPv4;
    handler->receive_handler(SOCKET_ERROR_NONE);
    CHECK(observer->dataAvailable == true);

    observer->dataAvailable == false;
    handler->_network_stack = M2MInterface::Nanostack_IPv6;
    handler->receive_handler(SOCKET_ERROR_NONE);
    CHECK(observer->dataAvailable == true);

    handler->receive_handler(SOCKET_ERROR_BAD_FAMILY);
    CHECK(observer->error == true);
}

void Test_M2MConnectionHandlerImpl_mbed::test_dns_handler()
{
    common_stub::event = (socket_event_t*)malloc(sizeof(socket_event_t));
    memset(common_stub::event, 0, sizeof(socket_event_t));
    handler->_network_stack = M2MInterface::LwIP_IPv4;

    handler->dns_handler(SOCKET_ERROR_NONE);
    CHECK(observer->addressReady == true);

    handler->_network_stack = M2MInterface::Nanostack_IPv6;
    handler->dns_handler(SOCKET_ERROR_NONE);
    CHECK(observer->addressReady == true);

    handler->dns_handler(SOCKET_ERROR_BAD_FAMILY);
    CHECK(observer->error == true);

    free(common_stub::event);
}

void Test_M2MConnectionHandlerImpl_mbed::test_error_handler()
{
    handler->error_handler(SOCKET_ERROR_BAD_FAMILY);
    CHECK(observer->error == true);
}

void Test_M2MConnectionHandlerImpl_mbed::test_get_network_stack()
{
    CHECK(M2MInterface::Uninitialized == handler->get_network_stack());
    handler->_network_stack = M2MInterface::LwIP_IPv4;
    CHECK(M2MInterface::LwIP_IPv4 == handler->get_network_stack());
}

void Test_M2MConnectionHandlerImpl_mbed::test_close_connection()
{
    // Empty function to cover the cases
    handler->close_connection();
}
