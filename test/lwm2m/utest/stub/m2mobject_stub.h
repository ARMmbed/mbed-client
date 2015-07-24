/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_OBJECT_STUB_H
#define M2M_OBJECT_STUB_H

#include "m2mobject.h"
#include "m2mobjectinstance.h"

//some internal test related stuff
namespace m2mobject_stub
{
    extern u_int16_t int_value;
    extern M2MObjectInstance* inst;
    extern bool bool_value;
    extern M2MObjectInstanceList instance_list;
    extern M2MBase::BaseType base_type;
    extern sn_coap_hdr_s *header;
    void clear();
}

#endif // M2M_OBJECT_STUB_H

