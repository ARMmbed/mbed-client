/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_RESOURCE_STUB_H
#define M2M_RESOURCE_STUB_H

#include "m2mresource.h"

//some internal test related stuff
namespace m2mresource_stub
{
    extern uint32_t int_value;
    extern uint8_t* value;
    extern bool bool_value;
    void clear();
}

#endif // M2M_RESOURCE_STUB_H

