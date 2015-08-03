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
#include "test_m2mconnectionsecuritypimpl_mbedtls.h"
#include "lwm2m-client/m2mtimerobserver.h"
#include "lwm2m-client/m2msecurity.h"
#include "m2msecurity_stub.h"
#include "mbedtls_stub.h"
#include "lwm2m-client/m2mconnectionhandler.h"
//#include "lwm2m-client-mbed/m2mconnectionhandlerpimpl.h"
#include "m2mtimer_stub.h"

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

Test_M2MConnectionSecurityPimpl::Test_M2MConnectionSecurityPimpl()
{
    mbedtls_stub::clear();
    m2msecurity_stub::clear();
}

Test_M2MConnectionSecurityPimpl::~Test_M2MConnectionSecurityPimpl()
{
}

void Test_M2MConnectionSecurityPimpl::test_constructor()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    CHECK(impl._timmer != NULL);
}

void Test_M2MConnectionSecurityPimpl::test_destructor()
{
    M2MConnectionSecurityPimpl* impl = new M2MConnectionSecurityPimpl();
    delete impl;
    //Memory leak detector will report an error if leaks
}

void Test_M2MConnectionSecurityPimpl::test_reset()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    impl._init_done = true;
    impl.reset();
    CHECK(impl._init_done == false);
}

void Test_M2MConnectionSecurityPimpl::test_init()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    CHECK( -1 == impl.init(NULL) );

    m2msecurity_stub::int_value = 5;
    M2MSecurity* sec = new M2MSecurity(M2MSecurity::Bootstrap);
    CHECK( -1 == impl.init(sec) );

    m2msecurity_stub::int_value = 5;
    m2msecurity_stub::has_value = true;
    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = -1;
    m2msecurity_stub::int_value = M2MSecurity::Psk;
    CHECK( -1 == impl.init(sec) );

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    m2msecurity_stub::int_value = 5;
    m2msecurity_stub::has_value = true;
    mbedtls_stub::expected_int = -1;
    mbedtls_stub::crt_expected_int = -1;
    CHECK( -1 == impl.init(sec) );

    mbedtls_stub::crt_expected_int = 0;
    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    m2msecurity_stub::int_value = M2MSecurity::Psk;
    CHECK( 0 == impl.init(sec) );

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = -1;
    m2msecurity_stub::int_value = M2MSecurity::Certificate;
    CHECK( -1 == impl.init(sec) );

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    m2msecurity_stub::int_value = M2MSecurity::Certificate;
    CHECK( -1 == impl.init(sec) );

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    mbedtls_stub::retArray[2] = 0;
    m2msecurity_stub::int_value = M2MSecurity::Certificate;
    CHECK( -1 == impl.init(sec) );

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    mbedtls_stub::retArray[2] = 0;
    mbedtls_stub::retArray[3] = 0;
    m2msecurity_stub::int_value = M2MSecurity::Certificate;
    CHECK( 0 == impl.init(sec) );

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    mbedtls_stub::retArray[2] = 0;
    mbedtls_stub::retArray[3] = 0;
    m2msecurity_stub::int_value = 99;
    CHECK( 0 == impl.init(sec) );

    delete sec;
}

void Test_M2MConnectionSecurityPimpl::test_connect()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    CHECK( -1 == impl.connect(NULL));

    impl._init_done = true;
    CHECK( -1 == impl.connect(NULL));

    m2msecurity_stub::int_value = 5;
    m2msecurity_stub::has_value = true;

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    CHECK( -1 == impl.connect(NULL));

    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    CHECK( -1 == impl.connect(NULL));

    m2mtimer_stub::bool_value = true;

    TestObserver obs;
    M2MConnectionHandler* obj = new M2MConnectionHandler(obs, NULL, M2MInterface::LwIP_IPv4);

    mbedtls_stub::expected_uint32_value = 12;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    mbedtls_stub::retArray[2] = MBEDTLS_ERR_SSL_WANT_READ;
    mbedtls_stub::retArray[3] = MBEDTLS_ERR_SSL_WANT_WRITE;
    mbedtls_stub::retArray[4] = 0;
    CHECK( -1 == impl.connect(obj));

    delete obj;

    mbedtls_stub::counter = 0;
    mbedtls_stub::expected_uint32_value = 0;
    CHECK( 0 == impl.connect(NULL));
}

void Test_M2MConnectionSecurityPimpl::test_start_connecting_non_blocking()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    CHECK( -1 == impl.start_connecting_non_blocking(NULL));

    m2mtimer_stub::total_bool_value = false;
    impl._init_done = true;
    CHECK( -1 == impl.start_connecting_non_blocking(NULL));

    m2msecurity_stub::int_value = 5;
    m2msecurity_stub::has_value = true;

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    CHECK( -1 == impl.start_connecting_non_blocking(NULL));

    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    CHECK( -1 == impl.start_connecting_non_blocking(NULL));

    TestObserver obs;
    M2MConnectionHandler* obj = new M2MConnectionHandler(obs, NULL, M2MInterface::LwIP_IPv4);

    m2mtimer_stub::total_bool_value = true;
    mbedtls_stub::expected_uint32_value = 12;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = 0;
    mbedtls_stub::retArray[1] = 0;
    mbedtls_stub::retArray[2] = 0;
    mbedtls_stub::retArray[3] = 0;
    mbedtls_stub::retArray[4] = 0;
    CHECK( 1 == impl.start_connecting_non_blocking(obj));

    delete obj;

    mbedtls_stub::counter = 0;
    mbedtls_stub::expected_uint32_value = 0;
    mbedtls_stub::retArray[3] = -1;
    CHECK( -1 == impl.start_connecting_non_blocking(NULL));
}

void Test_M2MConnectionSecurityPimpl::test_continue_connecting()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();

    mbedtls_stub::expected_int = MBEDTLS_ERR_SSL_WANT_READ;
    CHECK( M2MConnectionHandler::CONNECTION_ERROR_WANTS_READ == impl.continue_connecting());

    mbedtls_stub::expected_int = -6;
    impl._ssl.state = MBEDTLS_SSL_HANDSHAKE_OVER;
    CHECK( 0 == impl.continue_connecting());
}

void Test_M2MConnectionSecurityPimpl::test_send_message()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    unsigned char msg[6] = "hello";
    CHECK( -1 == impl.send_message(msg, 5) );
    impl._init_done = true;

    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = MBEDTLS_ERR_SSL_WANT_READ;
    mbedtls_stub::retArray[1] = MBEDTLS_ERR_SSL_WANT_WRITE;
    mbedtls_stub::retArray[2] = 65;
    CHECK( 65 == impl.send_message(msg, 5) );
}

void Test_M2MConnectionSecurityPimpl::test_read()

{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    unsigned char msg[50];
    CHECK( 0 == impl.read(msg, 49));

    impl._init_done = true;
    mbedtls_stub::useCounter = true;
    mbedtls_stub::counter = 0;
    mbedtls_stub::retArray[0] = MBEDTLS_ERR_SSL_WANT_READ;
    mbedtls_stub::retArray[1] = MBEDTLS_ERR_SSL_WANT_WRITE;
    mbedtls_stub::retArray[2] = 25;
    CHECK( 25 == impl.read(msg, 49));
}

void Test_M2MConnectionSecurityPimpl::test_timer_expired()
{
    M2MConnectionSecurityPimpl impl = M2MConnectionSecurityPimpl();
    impl.timer_expired(M2MTimerObserver::Dtls);
    //Nothing to test for now
}
