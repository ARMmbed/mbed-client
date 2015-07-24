/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef COMMON_STUB_H
#define COMMON_STUB_H

#include <sys/types.h>
#include <netdb.h>
#include "UDPSocket.h"
#include "nsdlaccesshelper.h"
#include "ip6string.h"
#include "Ticker.h"

//some internal test related stuff
namespace common_stub
{
    extern socket_error_t error;
    extern socket_event_t* event;
    extern socket_addr* addr;
    extern bool visited;
    extern bool bool_value;
    extern void* void_value;
    extern int int_value;
    extern int int2_value;
    extern uint16_t uint_value;
    extern omalw_certificate_list_t *cert;
    extern addrinfo* addrinfo;
    extern sn_coap_hdr_s *coap_header;
    extern sn_nsdl_resource_info_s *resource;
    extern pthread_t thread;
    extern const char *char_value;
    void clear();
}

#endif // COMMON_STUB_H
