/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef THREAD_HELPER_STUB_H
#define THREAD_HELPER_STUB_H

#include "lwm2m-client-linux/source/include/threadhelper.h"

//some internal test related stuff
namespace threadhelper_stub
{
    extern bool visited;
    extern void* void_pointer;
    void clear();
}

#endif // THREAD_HELPER_STUB_H

