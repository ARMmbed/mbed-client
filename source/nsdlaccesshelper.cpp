/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "include/nsdlaccesshelper.h"
#include "include/m2mnsdlinterface.h"

#ifdef USE_LINUX
#include "include/m2mconnectionhandlerimpl_linux.h"
M2MTimerImpl  *__timer_impl = NULL;
M2MConnectionHandlerImpl *__connection_impl = NULL;
#endif

// callback function for NSDL library to call into
sn_nsdl_mem_s __sn_nsdl_memory;
M2MNsdlInterface  *__nsdl_interface = NULL;

uint8_t __nsdl_c_callback(sn_coap_hdr_s *received_coap_ptr,
                          sn_nsdl_addr_s *address,
                          sn_proto_info_s * proto)
{
    uint8_t status = 0;
    if(__nsdl_interface) {
        status = __nsdl_interface->resource_callback(received_coap_ptr, address, proto);
    }
    return status;
}

void* __nsdl_c_memory_alloc(uint16_t size)
{
    void * val = NULL;
    if(__nsdl_interface) {
        val = __nsdl_interface->memory_alloc(size);
    }
    return val;
}

void __nsdl_c_memory_free(void *ptr)
{
    if(__nsdl_interface) {
        __nsdl_interface->memory_free(ptr);
    }
}

uint8_t __nsdl_c_send_to_server(sn_nsdl_capab_e protocol,
                                uint8_t *data_ptr,
                                uint16_t data_len,
                                sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    if(__nsdl_interface) {
        status = __nsdl_interface->send_to_server_callback(protocol, data_ptr,
                                                          data_len, address_ptr);
    }
    return status;
}

uint8_t __nsdl_c_received_from_server(sn_coap_hdr_s *coap_header,
                                      sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    if(__nsdl_interface) {
        status = __nsdl_interface->received_from_server_callback(coap_header,
                                                                 address_ptr);
    }
    return status;
}

void __nsdl_c_bootstrap_done(sn_nsdl_oma_server_info_t *server_info_ptr)
{
    if(__nsdl_interface) {
        __nsdl_interface->bootstrap_done_callback(server_info_ptr);
    }
}

void* __socket_malloc( void * context, size_t size)
{
    (void) context;
    return malloc(size);
}

void __socket_free(void * context, void * ptr)
{
    (void) context;
    free(ptr);
}
#ifdef USE_LINUX
void* __thread_poll_function(void* object)
{
    if(__timer_impl) {
       __timer_impl->thread_function(object);
    }
    return NULL;
}

void* __listen_data_function(void*)
{
    if(__connection_impl) {
       __connection_impl->data_receive();
    }
    return NULL;
}
#endif
