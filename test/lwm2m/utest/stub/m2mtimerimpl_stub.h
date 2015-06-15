/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_IMPL_STUB_H
#define M2M_TIMER_IMPL_STUB_H

#ifdef TARGET_LIKE_LINUX
    #include "lwm2m-client-linux/lwm2m-client-linux/m2mtimerimpl.h"
#else
    #include "lwm2m-client-mbed/lwm2m-client-mbed/m2mtimerimpl.h"
#endif

//some internal test related stuff
namespace m2mtimerimpl_stub
{
    extern bool visited;
    void clear();
}

#endif // M2M_TIMER_IMPL_STUB_H

