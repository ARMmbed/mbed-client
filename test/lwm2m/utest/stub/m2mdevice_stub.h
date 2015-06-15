/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_DEVICE_STUB_H
#define M2M_DEVICE_STUB_H

#include "m2mdevice.h"

//some internal test related stuff
namespace m2mdevice_stub
{
    extern uint32_t int_value;
    extern M2MResource* resource;
    extern String string_value;
    extern bool bool_value;
    void clear();
}

#endif // M2M_DEVICE_STUB_H

