/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_RESOURCE_INSTANCE_STUB_H
#define M2M_RESOURCE_INSTANCE_STUB_H

#include "m2mresourceinstance.h"

//some internal test related stuff
namespace m2mresourceinstance_stub
{
    extern uint32_t int_value;
    extern uint8_t* value;
    extern bool bool_value;
    extern M2MResourceInstance::ResourceType resource_type;
    extern sn_coap_hdr_s *header;
    void clear();
}

#endif // M2M_RESOURCE_INSTANCE_STUB_H

