/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef M2M_CORE_MEMORY_H
#define M2M_CORE_MEMORY_H

#include <inttypes.h>
#include "mbed-client-libservice/m2mdynmemLIB.h"

#define M2M_CORE_DYNMEM_LIB
//#define M2M_CORE_PASSTHROUGH
//#define M2M_CORE_TRACE_PRINTS

#define M2M_CORE_DYNAMIC_MEMORY_HEAP_SIZE 20000
class M2MCoreMemory {
public:
    static int memTotal;
    static int memCount;

#if (defined(M2M_CORE_DYNMEM_LIB)||defined(M2M_CORE_PASSTHROUGH))
    void * operator new (size_t size);
    void operator delete (void * ptr);
    void * operator new[] (size_t size);
    void operator delete[] (void * ptr);
#endif
#ifdef M2M_CORE_DYNMEM_LIB
    static void init(void);
    static void init(size_t size);
    static void init(void *heapAllocation, size_t size);
#endif
#ifdef M2M_CORE_TRACE_PRINTS
    static void print_heap_running_statistics(void);
    static void print_heap_overall_statistics(void);
#endif
    static void *memory_alloc(size_t size);
    static void memory_free(void *ptr);

    /* TODO at least for completeness there should be de-init method */

private:

#ifdef M2M_CORE_DYNMEM_LIB
    static void * heapPtr;
#ifdef M2M_CORE_TRACE_PRINTS
    static m2m_mem_stat_t memInfo;
    static void memory_fail_callback(m2m_heap_fail_t fail);
#endif
#endif
};

#endif
