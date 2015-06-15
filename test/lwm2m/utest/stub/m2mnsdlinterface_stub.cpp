/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mnsdlinterface_stub.h"

bool m2mnsdlinterface_stub::bool_value;
uint32_t m2mnsdlinterface_stub::int_value;

void m2mnsdlinterface_stub::clear()
{
    bool_value = false;
    int_value = 0;
}

M2MNsdlInterface::M2MNsdlInterface(M2MNsdlObserver &observer)
: _observer(observer)
{
}

M2MNsdlInterface::~M2MNsdlInterface()
{
}

bool M2MNsdlInterface::initialize()
{
    return m2mnsdlinterface_stub::bool_value;
}

void M2MNsdlInterface::create_endpoint(const String &,
                                       const String &,
                                       const int32_t,
                                       const String &,
                                       const uint8_t,
                                       const String &)
{
}

void M2MNsdlInterface::delete_endpoint()
{
}

bool M2MNsdlInterface::create_nsdl_list_structure(const M2MObjectList &)
{
    return m2mnsdlinterface_stub::bool_value;
}

bool M2MNsdlInterface::delete_nsdl_resource(const String &)
{
    return m2mnsdlinterface_stub::bool_value;
}

bool M2MNsdlInterface::create_bootstrap_resource(sn_nsdl_addr_s *)
{
    return m2mnsdlinterface_stub::bool_value;
}

bool M2MNsdlInterface::send_register_message(uint8_t*,
                                             const uint16_t,
                                             sn_nsdl_addr_type_e)
{
    return m2mnsdlinterface_stub::bool_value;
}

bool M2MNsdlInterface::send_update_registration(const uint32_t)
{
    return m2mnsdlinterface_stub::bool_value;
}

bool M2MNsdlInterface::send_unregister_message()
{
    return m2mnsdlinterface_stub::bool_value;
}

void *M2MNsdlInterface::memory_alloc(uint16_t size)
{
    return malloc(size);
}

void M2MNsdlInterface::memory_free(void *ptr)
{
    free(ptr);
    ptr = NULL;
}

uint8_t M2MNsdlInterface::send_to_server_callback(struct nsdl_s *,
                                                  sn_nsdl_capab_e,
                                                  uint8_t *,
                                                  uint16_t,
                                                  sn_nsdl_addr_s *)
{
    return (uint8_t)m2mnsdlinterface_stub::int_value;
}

uint8_t M2MNsdlInterface::received_from_server_callback(struct nsdl_s *,
                                                        sn_coap_hdr_s *,
                                                        sn_nsdl_addr_s *)
{
    return (uint8_t)m2mnsdlinterface_stub::int_value;
}

uint8_t M2MNsdlInterface::resource_callback(struct nsdl_s *,
                                            sn_coap_hdr_s *,
                                            sn_nsdl_addr_s *,
                                            sn_nsdl_capab_e)
{
    return (uint8_t)m2mnsdlinterface_stub::int_value;
}

void M2MNsdlInterface::bootstrap_done_callback(sn_nsdl_oma_server_info_t *)
{
}

bool M2MNsdlInterface::process_received_data(uint8_t *,
                                             uint16_t ,
                                             sn_nsdl_addr_s *)
{
    return m2mnsdlinterface_stub::bool_value;
}

void M2MNsdlInterface::timer_expired(M2MTimerObserver::Type)
{
}

void M2MNsdlInterface::observation_to_be_sent(M2MBase *)
{
}

void M2MNsdlInterface::resource_to_be_deleted(const String &)
{
}

void M2MNsdlInterface::remove_object(M2MBase *)
{
}
