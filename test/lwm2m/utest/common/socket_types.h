/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef LIBRARIES_MBED_HAL_SOCKET_TYPES_H_
#define LIBRARIES_MBED_HAL_SOCKET_TYPES_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SOCKET_ERROR_NONE = 0,
    SOCKET_ERROR_UNKNOWN,
    SOCKET_ERROR_BUSY,
    SOCKET_ERROR_NULL_PTR,
    SOCKET_ERROR_BAD_FAMILY,
    SOCKET_ERROR_TIMEOUT,
    SOCKET_ERROR_BAD_ALLOC,
    SOCKET_ERROR_NO_CONNECTION,
    SOCKET_ERROR_SIZE,
    SOCKET_ERROR_BAD_BUFFER,
    SOCKET_ERROR_STACK_EXISTS,
    SOCKET_ERROR_STACKS,
    SOCKET_ERROR_BAD_STACK,
    SOCKET_ERROR_BAD_ADDRESS,
    SOCKET_ERROR_DNS_FAILED

} socket_error_t;

typedef enum {
    SOCKET_ALLOC_HEAP = 0,
    SOCKET_ALLOC_POOL_BEST
} socket_alloc_pool_t;

//typedef enum {
//    // TBD
//} socket_flags_t;

typedef enum {
    SOCKET_DGRAM = 1,
    SOCKET_STREAM,
    SOCKET_RAW
} socket_proto_family_t;

typedef enum {
    SOCKET_EVENT_NONE = 0,
    SOCKET_EVENT_ERROR,
    SOCKET_EVENT_RX_DONE,
    SOCKET_EVENT_TX_DONE,
    SOCKET_EVENT_RX_ERROR,
    SOCKET_EVENT_TX_ERROR,
    SOCKET_EVENT_CONNECT,
    SOCKET_EVENT_DISCONNECT,
    SOCKET_EVENT_DNS
} event_flag_t;

typedef enum {
    SOCKET_STATUS_IDLE = 0,
    SOCKET_STATUS_RX_BUSY = 1 << 0,
    SOCKET_STATUS_TX_BUSY = 1 << 1,
    SOCKET_STATUS_CONNECTED = 1 << 2
} socket_status_t;

typedef enum {
    SOCKET_STACK_UNINIT = 0,
    SOCKET_STACK_LWIP_IPV4,
    SOCKET_STACK_LWIP_IPV6,
    SOCKET_STACK_RESERVED,
    SOCKET_STACK_NANOSTACK_IPV6,
    SOCKET_STACK_MAX
} socket_stack_t;

typedef enum {
    SOCKET_BUFFER_UNINITIALISED = 0,
    SOCKET_BUFFER_RAW,
    SOCKET_BUFFER_LWIP_PBUF,
    SOCKET_BUFFER_NANOSTACK_PBUF
} socket_buffer_type_t;

struct socket_addr {
    socket_stack_t type;
    void *impl;
};

struct socket_buffer {
    socket_buffer_type_t type;
    uint32_t flags;
    const struct socket_buf_api *api;
    void *impl;
};

struct socket_rawbuf {
    void * buf;
    size_t size;
    size_t pos;
};

typedef void (*socket_api_handler_t)(void);
typedef struct {
    void * (*alloc)(void *,const size_t);
    void (*dealloc)(void *, void *);
    void *context;
} socket_allocator_t;

struct socket_recv_info {
    void *context;
    struct socket *sock;
    struct socket_addr src;
    uint16_t port;
    struct socket_buffer buf;
    uint8_t free_buf;
};

struct socket_tx_info {
    void *context;
    struct socket *sock;
    struct socket_buffer *buf;
    uint16_t sentbytes;
    uint8_t free_buf;
};

struct socket_dns_info {
  struct socket *sock;
  struct socket_addr addr; // A stack-specific socket address struct
  const char *domain;
};
struct socket_event {
    event_flag_t event;
    union {
        struct socket_recv_info r;
        struct socket_tx_info t;
        socket_error_t e;
        struct socket_dns_info d;
    } i;
};
typedef struct socket_event socket_event_t;

enum socket_address_family_t {
    SOCKET_AF_INET4
};

struct socket {
    void *handler;
    socket_status_t status;
    uint8_t family;
    socket_event_t *event; // TODO: (CThunk upgrade/Alpha2)
    socket_stack_t stack;
    const struct socket_api *api;
    void *impl;
};

// TODO: The type of handler_t is TBD.
// NOTE: Since handlers are passed using C++ references, a global null handler will be provided so that empty handlers
// are provided for.  Overriding the null handler is likely to be useful for debugging.
/*
 * std::function, in combination with std::bind appear to be the ideal solution for handler_t, however
 * std::bind, is not supported by all our targets. We might be able to work around this by porting
 * boost::bind to mbed
 */
//typedef std::function<void(event_t*)> handler_t;
/*
 * We already have a feature in mbed for handling function pointers.
 */
//#include "FunctionPointer.h"
//typedef FunctionPointer handler_t
/*
 * Our fallback alternative for handler_t is a function pointer:
 */
typedef void (*handler_t)(void *);

#ifdef __cplusplus
}
#endif
#endif /* LIBRARIES_MBED_HAL_SOCKET_TYPES_H_ */
