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

#ifndef M2M_APPLICATION_MEMORY_H
#define M2M_APPLICATION_MEMORY_H

#include <inttypes.h>

#define M2M_DYNAMIC_MEMORY_HEAP_SIZE 20000
class M2MApplicationMemory {
public:
    static int memTotal;
    static int memCount;

    void * operator new (size_t size);
    void operator delete (void * ptr);
    void * operator new[] (size_t size);
    void operator delete[] (void * ptr);

    static void init(void);
    static void init(size_t size);
    static void init(void *heapAllocation, size_t size);
    static void print_heap_running_statistics(void);
    static void print_heap_overall_statistics(void);
    static void *memory_alloc(uint16_t size);
    static void memory_free(void *ptr);

    /* TODO at least for completeness there should be de-init method */

private:

    static void * heap;
    static void * heapPtr;
    static size_t heapSize;
    static int referenceCount;
};

#endif
