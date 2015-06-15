/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandler.h"
#include "m2mconnectionhandlerimpl_mbed_stub.h"
#include "m2mconnectionobserver.h"


class TestObserver : public M2MConnectionObserver {

public:
    TestObserver(){}
    void data_available(uint8_t*,
                        uint16_t,
                        const M2MConnectionObserver::SocketAddress &){}

    void socket_error(uint8_t error_code){}

    void address_ready(const M2MConnectionObserver::SocketAddress &,
                       M2MConnectionObserver::ServerType,
                       const uint16_t){}

    void data_sent(){}
};

Test_M2MConnectionHandler::Test_M2MConnectionHandler()
{
    TestObserver observer;
    handler = new M2MConnectionHandler(observer,M2MInterface::Uninitialized);
}

Test_M2MConnectionHandler::~Test_M2MConnectionHandler()
{
    m2mconnectionhandlerimpl_stub::clear();
    delete handler;
}

void Test_M2MConnectionHandler::test_bind_connection()
{
    m2mconnectionhandlerimpl_stub::bool_value = true;
    uint16_t listen_port = 5600;
    CHECK(handler->bind_connection(listen_port) ==true);
}

void Test_M2MConnectionHandler::test_resolve_server_address()
{    
    m2mconnectionhandlerimpl_stub::bool_value = true;
    String server_address = String("10.45.3.84");
    uint16_t server_port = 5600;
    M2MConnectionObserver::ServerType server_type = M2MConnectionObserver::Bootstrap;
    CHECK(handler->resolve_server_address(server_address,
                                server_port,
                                server_type) == true);
}

void Test_M2MConnectionHandler::test_send_data()
{
    m2mconnectionhandlerimpl_stub::bool_value = true;
    uint8_t *data_ptr = NULL;
    uint16_t data_len;
    sn_nsdl_addr_s *address_ptr = NULL;
    CHECK(handler->send_data(data_ptr,data_len,address_ptr) == true);
}

void Test_M2MConnectionHandler::test_listen_for_data()
{
    m2mconnectionhandlerimpl_stub::bool_value = true;
    CHECK(handler->listen_for_data() == true);
}

void Test_M2MConnectionHandler::test_close_connection()
{
    // Verifies the code coverage
    handler->close_connection();
}
