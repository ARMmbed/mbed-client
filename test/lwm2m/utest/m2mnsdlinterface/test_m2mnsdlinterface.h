/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_M2M_NSDL_INTERFACE_H
#define TEST_M2M_NSDL_INTERFACE_H

#include "m2mnsdlinterface.h"
#include "common_stub.h"

class TestObserver;

class Test_M2MNsdlInterface
{
public:
    Test_M2MNsdlInterface();

    virtual ~Test_M2MNsdlInterface();

    void test_create_endpoint();

    void test_delete_endpoint();

    void test_create_nsdl_list_structure();

    void test_delete_nsdl_resource();

    void test_create_bootstrap_resource();

    void test_send_register_message();

    void test_send_update_registration();

    void test_send_unregister_message();

    void test_memory_alloc();

    void test_memory_free();

    void test_send_to_server_callback();

    void test_received_from_server_callback();

    void test_resource_callback_get();

    void test_resource_callback_put();

    void test_resource_callback_post();

    void test_bootstrap_done_callback();

    void test_process_received_data();

    void test_timer_expired();

    void test_observation_to_be_sent();

    void test_resource_to_be_deleted();

    void test_remove_object();

    void test_add_object_to_list(); //Special: Would be too difficult to test in normal ways

    M2MNsdlInterface* nsdl;

    TestObserver *observer;
};

#endif // TEST_M2M_NSDL_INTERFACE_H
