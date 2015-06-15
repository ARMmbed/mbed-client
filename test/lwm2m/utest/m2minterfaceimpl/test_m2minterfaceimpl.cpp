/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2minterfaceimpl.h"
#include "m2minterfaceobserver.h"
#include "m2mconnectionhandler_stub.h"
#include "m2msecurity_stub.h"
#include "m2mnsdlinterface_stub.h"
#include "m2mobject_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mbase.h"

class TestObserver : public M2MInterfaceObserver {

public:
    TestObserver(){}
    virtual ~TestObserver(){}
    void bootstrap_done(M2MSecurity */*server_object*/){
        bootstrapped = true;
    }
    void object_registered(M2MSecurity */*security_object*/,
                           const M2MServer &/*server_object*/) {
        registered = true;
    }

    void object_unregistered(M2MSecurity */*server_object*/){
        unregistered = true;
    }

    void registration_updated(M2MSecurity */*security_object*/,
                              const M2MServer &/*server_object*/){
        registered = true;
    }

    void error(M2MInterface::Error /*error*/){
        error_occured = true;
    }

    void value_updated(M2MBase *, M2MBase::BaseType ){
        val_updated = true;
    }

    bool val_updated;
    bool error_occured;
    bool registered;
    bool unregistered;
    bool bootstrapped;
};

class M2MBaseTest : public M2MBase
{
public:
    M2MBaseTest() : M2MBase("a", M2MBase::Static){}
};

Test_M2MInterfaceImpl::Test_M2MInterfaceImpl()
{
    observer = new TestObserver();
    impl = new M2MInterfaceImpl(*observer,
                                "endpoint_name",
                                "endpoint_type",
                                120,
                                8000,
                                "domain");
}

Test_M2MInterfaceImpl:: ~Test_M2MInterfaceImpl()
{    
    delete observer;
    delete impl;
}

void Test_M2MInterfaceImpl::test_bootstrap()
{
    M2MSecurity *sec = new M2MSecurity(M2MSecurity::Bootstrap);
    String *val = new String("coap://10.45.3.83:5685");
    m2msecurity_stub::string_value = val;
    m2mconnectionhandler_stub::bool_value = true;

    impl->bootstrap(sec);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_BOOTSTRAP);

    impl->_current_state =  M2MInterfaceImpl::STATE_IDLE;
    m2mconnectionhandler_stub::bool_value = false;

    impl->bootstrap(sec);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    // Ignore the event.
    impl->_current_state =  M2MInterfaceImpl::STATE_WAITING;
    impl->bootstrap(sec);

    m2mconnectionhandler_stub::bool_value = true;

    impl->bootstrap(sec);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);
    CHECK(observer->error_occured == true);

    delete val;
    val = NULL;

    delete sec;
}

void Test_M2MInterfaceImpl::test_cancel_bootstrap()
{
    // Check for coverage, currently no implementation
    impl->cancel_bootstrap();
}

void Test_M2MInterfaceImpl::test_register_object()
{
    M2MSecurity *sec = new M2MSecurity(M2MSecurity::M2MServer);

    M2MObject *object = new M2MObject("test");
    M2MObjectInstance *ins = object->create_object_instance();
    ins->create_dynamic_resource("test","type",false);

    M2MObjectList list;
    list.push_back(object);

    String *val = new String("coap://10.45.3.83:5685");
    m2msecurity_stub::string_value = val;
    m2mnsdlinterface_stub::bool_value = true;
    m2mconnectionhandler_stub::bool_value = true;

    impl->register_object(sec,list);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_REGISTER);

    impl->_current_state =  M2MInterfaceImpl::STATE_IDLE;
    m2mconnectionhandler_stub::bool_value = true;
    m2mnsdlinterface_stub::bool_value = false;

    impl->register_object(sec,list);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    impl->_current_state =  M2MInterfaceImpl::STATE_IDLE;
    m2mconnectionhandler_stub::bool_value = false;
    m2mnsdlinterface_stub::bool_value = true;

    impl->register_object(sec,list);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    impl->_current_state =  M2MInterfaceImpl::STATE_IDLE;
    m2mconnectionhandler_stub::bool_value = true;
    m2mnsdlinterface_stub::bool_value = true;

    impl->register_object(sec,list);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_REGISTER);
    CHECK(observer->error_occured == true);

    impl->_current_state =  M2MInterfaceImpl::STATE_BOOTSTRAP;
    m2mconnectionhandler_stub::bool_value = true;
    m2mnsdlinterface_stub::bool_value = true;

    impl->register_object(sec,list);

    CHECK(observer->error_occured == true);


    delete val;
    val = NULL;

    list.clear();
    delete object;
    delete sec;
}

void Test_M2MInterfaceImpl::test_update_registration()
{    
    impl->_current_state = M2MInterfaceImpl::STATE_REGISTERED;
    m2mnsdlinterface_stub::bool_value = true;
    impl->update_registration(NULL,120);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_UPDATE_REGISTRATION);

    impl->_current_state = M2MInterfaceImpl::STATE_REGISTERED;
    m2mnsdlinterface_stub::bool_value = false;
    impl->update_registration(NULL,120);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);

    impl->_current_state = M2MInterfaceImpl::STATE_IDLE;
    impl->update_registration(NULL,120);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);
}

void Test_M2MInterfaceImpl::test_unregister_object()
{
    impl->_current_state = M2MInterfaceImpl::STATE_REGISTERED;
    m2mnsdlinterface_stub::bool_value = true;

    impl->unregister_object(NULL);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);

    impl->_current_state = M2MInterfaceImpl::STATE_REGISTERED;
    m2mnsdlinterface_stub::bool_value = false;

    impl->unregister_object(NULL);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    impl->_current_state = M2MInterfaceImpl::STATE_IDLE;

    impl->unregister_object(NULL);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);
}

void Test_M2MInterfaceImpl::test_coap_message_ready()
{
    m2mconnectionhandler_stub::bool_value = true;
    uint8_t *data_ptr = (uint8_t *)malloc(sizeof(uint8_t));
    uint16_t data_len = sizeof(uint8_t);
    sn_nsdl_addr_s *address_ptr = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));

    impl->coap_message_ready(data_ptr,data_len,address_ptr);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);

    m2mconnectionhandler_stub::bool_value = false;
    impl->coap_message_ready(data_ptr,data_len,address_ptr);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    free(address_ptr);
    free(data_ptr);
}

void Test_M2MInterfaceImpl::test_client_registered()
{
    impl->client_registered(NULL);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_REGISTERED);
    CHECK(observer->registered == true);
}

void Test_M2MInterfaceImpl::test_registration_updated()
{
    M2MServer *server = NULL;
    impl->registration_updated(*server);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_REGISTERED);
    CHECK(observer->registered == true);
}

void Test_M2MInterfaceImpl::test_registration_error()
{
    impl->registration_error(1);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);
}

void Test_M2MInterfaceImpl::test_client_unregistered()
{
    impl->client_unregistered();

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->unregistered == true);
}

void Test_M2MInterfaceImpl::test_bootstrap_done()
{
    M2MSecurity *sec = new M2MSecurity(M2MSecurity::M2MServer);
    impl->bootstrap_done(sec);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_BOOTSTRAPPED);
    CHECK(observer->bootstrapped == true);

    delete sec;
}

void Test_M2MInterfaceImpl::test_bootstrap_error()
{
    impl->bootstrap_error();

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);
}

void Test_M2MInterfaceImpl::test_coap_data_processed()
{
    impl->coap_data_processed();
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);
}

void Test_M2MInterfaceImpl::test_value_updated()
{
    impl->value_updated(NULL);
    CHECK(observer->val_updated == false);

    M2MBaseTest b;
    impl->value_updated(&b);
    CHECK(observer->val_updated == true);
}

void Test_M2MInterfaceImpl::test_data_available()
{
    uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t));
    uint16_t data_size = sizeof(uint8_t);
    M2MConnectionObserver::SocketAddress *address = (M2MConnectionObserver::SocketAddress*)
                                                    malloc(sizeof(M2MConnectionObserver::SocketAddress));

    address->_stack = M2MInterface::LwIP_IPv4;
    m2mnsdlinterface_stub::bool_value = true;

    impl->data_available(data,data_size,*address);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);

    address->_stack = M2MInterface::LwIP_IPv6;
    m2mnsdlinterface_stub::bool_value = true;

    impl->data_available(data,data_size,*address);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);

    address->_stack = M2MInterface::LwIP_IPv4;
    m2mnsdlinterface_stub::bool_value = false;

    impl->data_available(data,data_size,*address);

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);
    CHECK(observer->error_occured == true);

    free(data);
    free(address);
}

void Test_M2MInterfaceImpl::test_socket_error()
{
    impl->socket_error(2);

    CHECK(observer->error_occured == true);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);

    observer->error_occured = false;

    impl->socket_error(1);
    CHECK(observer->error_occured == true);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
}

void Test_M2MInterfaceImpl::test_address_ready()
{
    M2MConnectionObserver::SocketAddress *address =
        (M2MConnectionObserver::SocketAddress*) malloc(sizeof(M2MConnectionObserver::SocketAddress));
    M2MConnectionObserver::ServerType server_type = M2MConnectionObserver::Bootstrap;
    uint16_t server_port = 5685;

    address->_stack = M2MInterface::LwIP_IPv6;
    m2mnsdlinterface_stub::bool_value = true;

    impl->address_ready(*address,server_type,server_port);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_BOOTSTRAP_RESOURCE_CREATED);

    address->_stack = M2MInterface::LwIP_IPv4;
    m2mnsdlinterface_stub::bool_value = true;

    impl->address_ready(*address,server_type,server_port);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_BOOTSTRAP_RESOURCE_CREATED);


    address->_stack = M2MInterface::LwIP_IPv6;
    m2mnsdlinterface_stub::bool_value = false;

    impl->address_ready(*address,server_type,server_port);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    // Test for Address resolving for LWM2M server
    server_type = M2MConnectionObserver::LWM2MServer;

    address->_stack = M2MInterface::LwIP_IPv6;
    m2mnsdlinterface_stub::bool_value = true;

    impl->address_ready(*address,server_type,server_port);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_REGISTER_RESOURCE_CREATED);

    address->_stack = M2MInterface::LwIP_IPv4;
    m2mnsdlinterface_stub::bool_value = true;

    impl->address_ready(*address,server_type,server_port);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_REGISTER_RESOURCE_CREATED);


    address->_stack = M2MInterface::LwIP_IPv6;
    m2mnsdlinterface_stub::bool_value = false;

    impl->address_ready(*address,server_type,server_port);
    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_IDLE);
    CHECK(observer->error_occured == true);

    free(address);
}

void Test_M2MInterfaceImpl::test_data_sent()
{
    m2mconnectionhandler_stub::bool_value = true;

    impl->data_sent();

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);

    m2mconnectionhandler_stub::bool_value = false;

    impl->data_sent();

    CHECK(impl->_current_state == M2MInterfaceImpl::STATE_WAITING);
    CHECK(observer->error_occured == false);

}
