/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mconnectionhandlerimpl_stub.h"
#include "common_stub.h"

uint16_t m2mconnectionhandlerimpl_stub::uint_value;
bool m2mconnectionhandlerimpl_stub::bool_value;

void m2mconnectionhandlerimpl_stub::clear()
{
    uint_value = 0;
    bool_value = false;    
}

M2MConnectionHandlerImpl::M2MConnectionHandlerImpl(M2MConnectionObserver &observer,
                                           M2MInterface::NetworkStack)
:_observer(observer)
{
}

M2MConnectionHandlerImpl::~M2MConnectionHandlerImpl()
{
}

bool M2MConnectionHandlerImpl::bind_connection(const uint16_t)
{
    return m2mconnectionhandlerimpl_stub::bool_value;
}

bool M2MConnectionHandlerImpl::resolve_server_address(const String&,
                                                  const uint16_t,
                                                  M2MConnectionObserver::ServerType)
{
    return m2mconnectionhandlerimpl_stub::bool_value;
}

bool M2MConnectionHandlerImpl::listen_for_data()
{
    return m2mconnectionhandlerimpl_stub::bool_value;
}

bool M2MConnectionHandlerImpl::send_data(uint8_t *,
                                     uint16_t ,
                                     sn_nsdl_addr_s *)
{
    return m2mconnectionhandlerimpl_stub::bool_value;
}

void M2MConnectionHandlerImpl::close_connection()
{
}

void M2MConnectionHandlerImpl::data_receive(void *)
{
}
