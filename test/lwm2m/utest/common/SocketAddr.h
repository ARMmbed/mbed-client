/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef SOCKET_ADDR_H
#define SOCKET_ADDR_H

#include "socket_types.h"

class SocketAddr {
public:
    const struct socket_addr * getAddr() const;
    void * getImpl();
    void setAddr(const struct socket_addr *addr);
    void setAddr(const SocketAddr *addr);
    size_t getAddrSize() const;
};

#endif
