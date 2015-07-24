/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_OBJECT_INSTANCE_STUB_H
#define M2M_OBJECT_INSTANCE_STUB_H

#include "m2mobjectinstance.h"

//some internal test related stuff
namespace m2mobjectinstance_stub
{
    extern u_int8_t int_value;
    extern bool bool_value;
    extern M2MResource *resource;
    extern M2MResource* create_resource;
    extern M2MResourceInstance* create_resource_instance;
    extern sn_coap_hdr_s* header;
    extern M2MResourceList resource_list;
    void clear();    
}

#endif // M2M_OBJECT_INSTANCE_STUB_H

