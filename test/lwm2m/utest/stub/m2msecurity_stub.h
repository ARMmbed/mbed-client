/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_SECURITY_STUB_H
#define M2M_SECURITY_STUB_H

#include "m2msecurity.h"

//some internal test related stuff
namespace m2msecurity_stub
{
    extern bool has_value;
    extern uint32_t int_value;
    extern bool bool_value;
    extern String *string_value;
    extern M2MResource* resource;
    void clear();
}

#endif // M2M_SECURITY_STUB_H

