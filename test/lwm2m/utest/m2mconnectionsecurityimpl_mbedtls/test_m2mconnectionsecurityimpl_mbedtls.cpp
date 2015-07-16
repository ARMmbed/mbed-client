/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mconnectionsecurityimpl_mbedtls.h"
#include "lwm2m-client/m2mtimerobserver.h"
#include "lwm2m-client/m2msecurity.h"
#include "m2msecurity_stub.h"
#include "mbedtls_stub.h"
#include "lwm2m-client/m2mconnectionhandler.h"
#include "lwm2m-client-mbed/m2mconnectionhandlerpimpl.h"
#include "m2mconnectionsecuritypimpl_stub.h"

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

Test_M2MConnectionSecurityImpl::Test_M2MConnectionSecurityImpl()
{
    mbedtls_stub::clear();
    m2msecurity_stub::clear();
}

Test_M2MConnectionSecurityImpl::~Test_M2MConnectionSecurityImpl()
{
}

void Test_M2MConnectionSecurityImpl::test_constructor()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
}

void Test_M2MConnectionSecurityImpl::test_destructor()
{
    M2MConnectionSecurity* impl = new M2MConnectionSecurity();
    delete impl;
    //Memory leak detector will report an error if leaks
}

void Test_M2MConnectionSecurityImpl::test_reset()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    impl.reset();
}

void Test_M2MConnectionSecurityImpl::test_init()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    m2mconnectionsecuritypimpl_stub::int_value = 7;
    CHECK( 7 == impl.init(NULL) );
}

void Test_M2MConnectionSecurityImpl::test_connect()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    m2mconnectionsecuritypimpl_stub::int_value = 7;
    CHECK( 7 == impl.connect(NULL));
}

void Test_M2MConnectionSecurityImpl::test_start_connecting_non_blocking()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    m2mconnectionsecuritypimpl_stub::int_value = 7;
    CHECK( 7 == impl.start_connecting_non_blocking(NULL));
}

void Test_M2MConnectionSecurityImpl::test_continue_connecting()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    m2mconnectionsecuritypimpl_stub::int_value = 7;
    CHECK( 7 == impl.continue_connecting());
}

void Test_M2MConnectionSecurityImpl::test_send_message()
{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    unsigned char msg[6] = "hello";
    m2mconnectionsecuritypimpl_stub::int_value = 7;
    CHECK( 7 == impl.send_message(msg, 5) );
}

void Test_M2MConnectionSecurityImpl::test_read()

{
    M2MConnectionSecurity impl = M2MConnectionSecurity();
    unsigned char msg[50];
    m2mconnectionsecuritypimpl_stub::int_value = 7;
    CHECK( 7 == impl.read(msg, 49));
}

