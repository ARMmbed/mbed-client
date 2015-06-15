/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2minterfacefactory.h"
#include "m2minterfaceobserver.h"
#include "m2mserver.h"
#include "m2mdevice.h"

class TestObserver : public M2MInterfaceObserver {

public:
    TestObserver(){}
    void bootstrap_done(M2MSecurity */*server_object*/){}
    void object_registered(M2MSecurity */*security_object*/,
                           const M2MServer &/*server_object*/) {}

    void object_unregistered(M2MSecurity */*server_object*/){}

    void registration_updated(M2MSecurity */*security_object*/,
                              const M2MServer &/*server_object*/){}

    void error(M2MInterface::Error /*error*/){}

    void value_updated(M2MBase */*base*/, M2MBase::BaseType /*type*/){}
};

Test_M2MInterfaceFactory::Test_M2MInterfaceFactory()
{
}

Test_M2MInterfaceFactory::~Test_M2MInterfaceFactory()
{

}

void Test_M2MInterfaceFactory::test_create_interface()
{
    TestObserver test_obs;
    M2MInterface *test = M2MInterfaceFactory::create_interface(test_obs,
                                                "endpoint_name",
                                                "endpoint_type",
                                                120,
                                                8000,
                                                "domain");
    CHECK(test != NULL);
    delete test;
}

void Test_M2MInterfaceFactory::test_create_security()
{
    M2MSecurity *test = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
    CHECK(test != NULL);
    delete test;

}

void Test_M2MInterfaceFactory::test_create_device()
{
    M2MDevice *test = M2MInterfaceFactory::create_device();
    CHECK(test != NULL);
    delete test;
}

void Test_M2MInterfaceFactory::test_create_server()
{
    M2MServer *test = M2MInterfaceFactory::create_server();
    CHECK(test != NULL);
    delete test;
}

void Test_M2MInterfaceFactory::test_create_object()
{
    M2MObject *test = M2MInterfaceFactory::create_object("name");
    CHECK(test != NULL);
    delete test;
}
