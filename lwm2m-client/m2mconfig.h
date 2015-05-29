/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2MCONFIG_H
#define M2MCONFIG_H

#include <stdlib.h>
#include "m2mstring.h"
using namespace m2m;
#ifdef TARGET_LIKE_LINUX
#include <cstring>
#include <cstdio>
#else
#include "mbed.h"
#endif

#ifdef HAVE_DEBUG
#define TRACE_GROUP "lwm2m-client"
#endif

#endif // M2MCONFIG_H
