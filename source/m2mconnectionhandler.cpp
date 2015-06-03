/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "include/m2mconnectionhandler.h"
#ifdef TARGET_LIKE_LINUX
    #include "lwm2m-client-linux/m2mconnectionhandlerimpl.h"
#else
    #include "lwm2m-client-mbed/m2mconnectionhandlerimpl.h"
#endif
#include "include/nsdlaccesshelper.h"
#include "lwm2m-client/m2mconstants.h"
#include "libService/ns_trace.h"

M2MConnectionHandler::M2MConnectionHandler(M2MConnectionObserver &observer,
                                           M2MInterface::NetworkStack stack)
:_observer(observer),
  _impl(new M2MConnectionHandlerImpl(_observer, stack))
{
    tr_debug("M2MConnectionHandler::M2MConnectionHandler()");
}

M2MConnectionHandler::~M2MConnectionHandler()
{
    tr_debug("M2MConnectionHandler::~M2MConnectionHandler()");
    delete _impl;
}

bool M2MConnectionHandler::bind_connection(const uint16_t listen_port)
{
    tr_debug("M2MConnectionHandler::bind_connection(port %d)", listen_port);
    return _impl->bind_connection(listen_port);
}

bool M2MConnectionHandler::resolve_server_address(const String& server_address,
                                                  const uint16_t server_port,
                                                  M2MConnectionObserver::ServerType server_type)
{
    tr_debug("M2MConnectionHandler::resolve_server_address(server_address %s, server_port %d, server_type %d)",
             server_address.c_str(),server_port,(int)server_type);
    return _impl->resolve_server_address(server_address,server_port,server_type);
}

bool M2MConnectionHandler::listen_for_data()
{
    tr_debug("M2MConnectionHandler::listen_for_data()");
    return _impl->listen_for_data();
}

bool M2MConnectionHandler::send_data(uint8_t *data,
                                     uint16_t data_len,
                                     sn_nsdl_addr_s *address)
{
    tr_debug("M2MConnectionHandler::send_data(data_len %d)", data_len);
    return _impl->send_data(data, data_len, address);
}
