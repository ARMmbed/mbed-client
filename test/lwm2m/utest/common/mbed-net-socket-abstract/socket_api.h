/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __MBED_NET_SOCKET_ABSTRACT_SOCKET_API_H__
#define __MBED_NET_SOCKET_ABSTRACT_SOCKET_API_H__

#include <stdint.h>
#include "socket_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOCKET_MAX_STACKS
#define SOCKET_MAX_STACKS 2
#endif

typedef void * (*socket_buf_get_ptr)(const struct socket_buffer *b);
typedef size_t (*socket_buf_get_size)(const struct socket_buffer *b);
typedef void (*socket_buf_alloc)(const size_t len, const socket_alloc_pool_t p, struct socket_buffer *b);
typedef socket_error_t (*socket_buf_try_free)(struct socket_buffer *b);
typedef void (*socket_buf_free)(struct socket_buffer *b);
typedef socket_error_t (*socket_copy_from_user)(struct socket_buffer *b, const void *u, const size_t len);
typedef uint16_t (*socket_copy_to_user)(void *u, const struct socket_buffer *b, const size_t len);

struct socket_buf_api {
    //socket_buf_stack_to_buf stack_to_buf;
    socket_buf_get_ptr      get_ptr;
    socket_buf_get_size     get_size;
    socket_buf_alloc        alloc;
    socket_buf_try_free     try_free;
    socket_buf_free         free;
    socket_copy_from_user   u2b;
    socket_copy_to_user     b2u;
};


typedef socket_error_t (*socket_init)();
typedef socket_error_t (*socket_create)(struct socket *socket,
		const socket_address_family_t af, const socket_proto_family_t pf,
		socket_api_handler_t const handler);
typedef socket_error_t (*socket_destroy)(struct socket *socket);
typedef socket_error_t (*socket_close)(struct socket *socket);
//typedef void (*socket_abort)(struct socket *socket);
typedef socket_api_handler_t (*socket_periodic_task)(const struct socket * socket);
typedef uint32_t (*socket_periodic_interval)(const struct socket * socket);
typedef socket_error_t (*socket_resolve)(struct socket *socket, const char *address);
typedef socket_error_t (*socket_connect)(struct socket *sock, const struct socket_addr *address, const uint16_t port);
typedef socket_error_t (*socket_bind)(struct socket *socket, const struct socket_addr *address, const uint16_t port);
typedef socket_error_t (*socket_str2addr)(const struct socket *socket, struct socket_addr *addr, const char *address);
typedef socket_error_t (*socket_start_listen)(struct socket *socket, const uint32_t backlog);
typedef socket_error_t (*socket_stop_listen)(struct socket *socket);
typedef socket_error_t (*socket_accept)(struct socket *sock, socket_api_handler_t handler);
//typedef socket_error_t (*socket_start_send)(struct socket *socket, struct socket_buffer *buf);
//typedef socket_error_t (*socket_start_sendv)(struct socket *socket, void *buf, size_t len);
//typedef socket_error_t (*socket_start_recv)(struct socket *socket);
typedef socket_error_t (*socket_send)(struct socket *socket, const void * buf, const size_t len);
typedef socket_error_t (*socket_send_to)(struct socket *socket, const void * buf, const size_t len, const struct socket_addr *addr, const uint16_t port);
typedef socket_error_t (*socket_recv)(struct socket *socket, void * buf, size_t *len);
typedef socket_error_t (*socket_recv_from)(struct socket *socket, void * buf, size_t *len, struct socket_addr *addr, uint16_t *port);


typedef uint8_t (*socket_is_connected)(const struct socket *socket);
typedef uint8_t (*socket_is_bound)(const struct socket *socket);
typedef uint8_t (*socket_tx_is_busy)(const struct socket *socket);
typedef uint8_t (*socket_rx_is_busy)(const struct socket *socket);

struct socket_api {
    socket_stack_t              stack;
    socket_init                 init;
//    struct socket_buf_api       buf_api;
    socket_create               create;
    socket_destroy              destroy;
    socket_close                close;
//    socket_abort                abort;
    socket_periodic_task        periodic_task;
    socket_periodic_interval    periodic_interval;
    socket_resolve              resolve;
    socket_connect              connect;
    socket_str2addr             str2addr;
    socket_bind                 bind;
    socket_start_listen         start_listen;
    socket_stop_listen          stop_listen;
    socket_accept               accept;
//    socket_start_send           start_send;
//    socket_start_sendv          start_sendv;
//    socket_start_recv           start_recv;
    socket_send                 send;
    socket_send_to              send_to;
    socket_recv                 recv;
    socket_recv_from            recv_from;
    socket_is_connected         is_connected;
    socket_is_bound             is_bound;
    socket_tx_is_busy           tx_busy;
    socket_rx_is_busy           rx_busy;
    socket_buffer_type_t        pbuf_type;
};
#define SOCKET_API_FIRST_PTR init
#define SOCKET_API_LAST_PTR rx_busy

socket_error_t socket_register_stack(const struct socket_api * api);
const char * socket_strerror(const socket_error_t err);
const struct socket_api * socket_get_api(const socket_stack_t stack);

#ifdef __cplusplus
}
#endif

#endif /* __MBED_NET_SOCKET_ABSTRACT_SOCKET_API_H__ */
