/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "nsdlaccesshelper_stub.h"

bool nsdlaccesshelper_stub::bool_value;
void* nsdlaccesshelper_stub::void_value;
uint8_t nsdlaccesshelper_stub::int_value;

M2MNsdlInterface  *__nsdl_interface= NULL;
#ifdef USE_LINUX
M2MTimerImpl  *__timer_impl = NULL;
M2MConnectionHandler *__connection_impl = NULL;
#endif
void nsdlaccesshelper_stub::clear()
{
    bool_value = false;
    void_value = NULL;
    int_value = 0;
}

uint8_t __nsdl_c_callback(struct nsdl_s * ,
                          sn_coap_hdr_s *,
                          sn_nsdl_addr_s *,
                          sn_nsdl_capab_e )
{
    return nsdlaccesshelper_stub::int_value;
}

void *__nsdl_c_memory_alloc(uint16_t)
{
    return nsdlaccesshelper_stub::void_value;
}

void __nsdl_c_memory_free(void *)
{
}

uint8_t __nsdl_c_send_to_server(struct nsdl_s * ,
                                sn_nsdl_capab_e ,
                                uint8_t *,
                                uint16_t ,
                                sn_nsdl_addr_s *)
{
    return nsdlaccesshelper_stub::int_value;
}

uint8_t __nsdl_c_received_from_server(struct nsdl_s * ,
                                      sn_coap_hdr_s *,
                                      sn_nsdl_addr_s *)
{
    return nsdlaccesshelper_stub::int_value;
}

void __nsdl_c_bootstrap_done(sn_nsdl_oma_server_info_t *)
{
}

void *__socket_malloc( void *, size_t)
{
return nsdlaccesshelper_stub::void_value;
}

void __socket_free(void *, void *)
{
}
