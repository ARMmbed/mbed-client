/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_HANDLER_STUB_H
#define M2M_CONNECTION_HANDLER_STUB_H

#include "m2mconnectionhandler.h"

//some internal test related stuff
namespace m2mconnectionhandler_stub
{
    extern uint8_t *int_value;
    extern String string_value;
    extern M2MInterface::NetworkStack stack;
    extern bool bool_value;
    void clear();
}

#endif // M2M_CONNECTION_HANDLER_STUB_H

