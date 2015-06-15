/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_BASE_STUB_H
#define M2M_BASE_STUB_H

#include "m2mbase.h"

//some internal test related stuff
namespace m2mbase_stub
{
    extern uint8_t uint8_value;
    extern uint16_t uint16_value;
    extern uint16_t int_value;
    extern String *string_value;
    extern uint8_t* value;
    extern bool bool_value;
    extern M2MBase::BaseType base_type;
    extern M2MBase::Mode mode_value;
    extern M2MBase::Operation operation;
    void clear();
}

#endif // M2M_BASE_STUB_H

