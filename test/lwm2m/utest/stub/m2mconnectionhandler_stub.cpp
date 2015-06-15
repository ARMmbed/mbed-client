/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mconnectionhandler_stub.h"
#include "common_stub.h"

uint8_t* m2mconnectionhandler_stub::int_value;
String m2mconnectionhandler_stub::string_value;
bool m2mconnectionhandler_stub::bool_value;
M2MInterface::NetworkStack m2mconnectionhandler_stub::stack;

void m2mconnectionhandler_stub::clear()
{
    int_value = NULL;
    bool_value = false;
    string_value = "";
    stack = M2MInterface::Uninitialized;
}

M2MConnectionHandler::M2MConnectionHandler(M2MConnectionObserver &observer,
                                           M2MInterface::NetworkStack)
:_observer(observer)
{
}

M2MConnectionHandler::~M2MConnectionHandler()
{
}

bool M2MConnectionHandler::bind_connection(const uint16_t)
{
    return m2mconnectionhandler_stub::bool_value;
}

bool M2MConnectionHandler::resolve_server_address(const String&,
                                                  const uint16_t,
                                                  M2MConnectionObserver::ServerType)
{
    return m2mconnectionhandler_stub::bool_value;
}

bool M2MConnectionHandler::listen_for_data()
{
    return m2mconnectionhandler_stub::bool_value;
}

bool M2MConnectionHandler::send_data(uint8_t *,
                                     uint16_t ,
                                     sn_nsdl_addr_s *)
{
    return m2mconnectionhandler_stub::bool_value;
}

void M2MConnectionHandler::close_connection()
{
}
