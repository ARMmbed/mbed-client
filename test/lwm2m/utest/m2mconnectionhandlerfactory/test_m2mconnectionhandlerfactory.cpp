/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionhandlerfactory.h"
#include "m2mconnectionhandler_stub.h"
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

Test_M2MConnectionHandlerFactory::Test_M2MConnectionHandlerFactory()
{

}

Test_M2MConnectionHandlerFactory::~Test_M2MConnectionHandlerFactory()
{

}

void Test_M2MConnectionHandlerFactory::test_createConnectionHandler()
{
    TestObserver obs;
    M2MConnectionHandler* obj = M2MConnectionHandlerFactory::createConnectionHandler(obs, M2MInterface::LwIP_IPv4);
    CHECK(obj != NULL );
    delete obj;
}
