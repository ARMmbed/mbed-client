/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerimpl_linux.h"
#include "m2mconnectionobserver.h"
#include "common_stub.h"

class M2MConnection_TestObserver : public M2MConnectionObserver {

public:
    M2MConnection_TestObserver():
    obj(NULL){}

    virtual ~M2MConnection_TestObserver(){}

    void set_class_object(M2MConnectionHandlerImpl *impl) {obj = impl;}
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
    M2MConnectionHandlerImpl *obj;
};

Test_M2MConnectionHandlerImpl_linux::Test_M2MConnectionHandlerImpl_linux()
{
    observer = new M2MConnection_TestObserver();
    common_stub::int_value = 0;
    common_stub::thread = 14;
    handler = new M2MConnectionHandlerImpl(*observer,M2MInterface::Uninitialized);
}

Test_M2MConnectionHandlerImpl_linux::~Test_M2MConnectionHandlerImpl_linux()
{
    delete handler;
    delete observer;
}

void Test_M2MConnectionHandlerImpl_linux::test_bind_connection()
{
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::int_value = 5;
    CHECK( handler->bind_connection(7) == true);
}

void Test_M2MConnectionHandlerImpl_linux::test_resolve_server_address()
{
    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer) == true);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;

    common_stub::error = SOCKET_ERROR_NONE;
    common_stub::addrinfo = (addrinfo*)malloc(sizeof(addrinfo));
    common_stub::addrinfo->ai_family = AF_INET6;
    common_stub::addrinfo->ai_addr = (sockaddr*)malloc(sizeof(sockaddr));
    handler->_socket_server = -1;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer) == true);
    free(common_stub::addrinfo->ai_addr);
    free(common_stub::addrinfo);
    common_stub::addrinfo = NULL;

    free(handler->_received_packet_address);
    handler->_received_packet_address = NULL;
    CHECK(handler->resolve_server_address("10", 7, M2MConnectionObserver::LWM2MServer) == false);
}

void Test_M2MConnectionHandlerImpl_linux::test_send_data()
{
    sn_nsdl_addr_s* addr = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(addr, 0, sizeof(sn_nsdl_addr_s));
    CHECK( false == handler->send_data(NULL, 0 , NULL));

    common_stub::int_value = 4;
    CHECK(true == handler->send_data(NULL, 0 , addr));

    common_stub::int_value = -1;
    CHECK(false == handler->send_data(NULL, 0 , addr));
    free(addr);
}

void Test_M2MConnectionHandlerImpl_linux::test_listen_for_data()
{
    handler->listen_for_data();
}

void Test_M2MConnectionHandlerImpl_linux::test_data_receive()
{
    handler->data_receive(NULL);

    M2MConnectionHandlerImpl *obj = new M2MConnectionHandlerImpl(*observer,M2MInterface::Uninitialized);

    handler->_receive_data = true;
    observer->error = false;
    common_stub::int_value = -1;
    handler->data_receive(obj);
    CHECK(observer->error == true);
    CHECK(handler->_receive_data == false);

    handler->_receive_data = true;
    observer->set_class_object(handler);
    observer->dataAvailable = false;
    common_stub::int_value = 6;
    handler->data_receive(obj);
    CHECK(observer->dataAvailable == true);

    delete obj;
    obj = NULL;
}

void Test_M2MConnectionHandlerImpl_linux::test_close_connection()
{
    handler->_receive_data == true;
    handler->close_connection();
    CHECK(handler->_receive_data == false);
}
