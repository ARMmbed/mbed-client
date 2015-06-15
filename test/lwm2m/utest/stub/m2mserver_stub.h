/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_SERVER_STUB_H
#define M2M_SERVER_STUB_H

#include "m2mserver.h"
#include "m2mresource.h"

//some internal test related stuff
namespace m2mserver_stub
{
    extern uint32_t int_value;
    extern bool bool_value;
    extern String string_value;
    extern M2MResource* resource;
    void clear();
}

#endif // M2M_SERVER_STUB_H

