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
#include "include/nsdlaccesshelper.h"
#include "include/m2mnsdlinterface.h"

// callback function for NSDL library to call into
M2MNsdlInterface  *__nsdl_interface = NULL;

uint8_t __nsdl_c_callback(struct nsdl_s *nsdl_handle,
                          sn_coap_hdr_s *received_coap_ptr,
                          sn_nsdl_addr_s *address,
                          sn_nsdl_capab_e nsdl_capab)
{
    uint8_t status = 0;
    if(__nsdl_interface) {
        status = __nsdl_interface->resource_callback(nsdl_handle,received_coap_ptr,
                                                     address, nsdl_capab);
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

uint8_t __nsdl_c_send_to_server(struct nsdl_s * nsdl_handle,
                                sn_nsdl_capab_e protocol,
                                uint8_t *data_ptr,
                                uint16_t data_len,
                                sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    if(__nsdl_interface) {
        status = __nsdl_interface->send_to_server_callback(nsdl_handle,
                                                           protocol, data_ptr,
                                                           data_len, address_ptr);
    }
    return status;
}

uint8_t __nsdl_c_received_from_server(struct nsdl_s * nsdl_handle,
                                      sn_coap_hdr_s *coap_header,
                                      sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    if(__nsdl_interface) {
        status = __nsdl_interface->received_from_server_callback(nsdl_handle,
                                                                 coap_header,
                                                                 address_ptr);
    }
    return status;
}

void __nsdl_c_bootstrap_done(sn_nsdl_oma_server_info_t *server_info_ptr)
{
#ifndef YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
    if(__nsdl_interface) {
        __nsdl_interface->bootstrap_done_callback(server_info_ptr);
    }
#endif //YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
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
