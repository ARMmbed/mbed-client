/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef MBEDTLS_STUB_H
#define MBEDTLS_STUB_H

#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy.h"
#include "mbedtls/pk.h"

namespace mbedtls_stub
{
    extern int crt_expected_int;
    extern bool useCounter;
    extern int counter;
    extern int retArray[20];
    extern int expected_int;
    extern uint32_t expected_uint32_value;
    void clear();
}

#endif // MBEDTLS_STUB_H
