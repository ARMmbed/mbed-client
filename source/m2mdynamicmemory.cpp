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

#include <stdlib.h>
#include "include/m2mdynamicmemory.h"
#include "include/m2mdynmemLIB.h"

int M2MDynamicMemory::memTotal = 0;
int M2MDynamicMemory::memCount = 0;
void * M2MDynamicMemory::heap = 0;
void * M2MDynamicMemory::heapPtr = 0;
int M2MDynamicMemory::referenceCount = 0;


#define M2M_DYNMEM_LIB
//#define M2M_PASSTHROUGH
//#define M2M_TRACE_PRINTS

#ifdef M2M_PASSTHROUGH
#ifdef M2M_TRACE_PRINTS
#include <malloc.h>
#endif
#endif

#ifdef M2M_TRACE_PRINTS
#include <stdio.h>
#endif

#ifdef M2M_DYNMEM_LIB
static mem_stat_t memInfo;
#endif
static void memory_fail_callback(heap_fail_t fail) {
#ifdef M2M_TRACE_PRINTS
    printf("\nM2M memory failure: %u\n", fail);
#endif
}

#ifdef M2M_DYNMEM_LIB

/* nanostack dynmemlib based implementation */
void * M2MDynamicMemory::operator new (size_t size) {
    void *tmp;
    tmp=m2m_dyn_mem_alloc(size);
#ifdef M2M_TRACE_PRINTS
    memTotal+=size; memCount++;
    printf("mn"); /* M2M new */
    print_heap_running_statistics();
#endif
    return tmp;
}

void M2MDynamicMemory::operator delete (void * ptr) {
#ifdef M2M_TRACE_PRINTS
    memCount--; /* still update allocation counter */
#endif
    m2m_dyn_mem_free(ptr);
#ifdef M2M_TRACE_PRINTS
    printf("md"); /* M2M delete */
    print_heap_overall_statistics();
#endif
}
void * M2MDynamicMemory::operator new[] (size_t size) {
    void *tmp;
    tmp=m2m_dyn_mem_alloc(size);
#ifdef M2M_TRACE_PRINTS
    memTotal+=size; memCount++;
    printf("mn[]"); /* M2M new array */
#endif
    return tmp;
}
void M2MDynamicMemory::operator delete[] (void * ptr) {
#ifdef M2M_TRACE_PRINTS
    memCount--;
#endif
    m2m_dyn_mem_free(ptr);
#ifdef M2M_TRACE_PRINTS
    printf("md[]"); /* M2M delete array */
#endif
}
#endif
#if 0
/* own very simple only alloc alloc */
void * M2MDynamicMemory::operator new (size_t size) {
    void *tmp;
    size_t allocatedSize;
    tmp=heapPtr;
    allocatedSize=((size-1) / 8 + 1) * 8; /* "worst case" 64-bit alignment assumption */
    heapPtr+=allocatedSize;
    memTotal+=allocatedSize; memCount++;
    printf("cn:%lu:%lu:%d:%d:%p:", size, allocatedSize, memTotal, memCount, tmp);
    return tmp;
}

void M2MDynamicMemory::operator delete (void * ptr) {
    memCount--; /* still update allocation counter */
    printf("cd:nop:%d", memCount);
}

void * M2MDynamicMemory::operator new[] (size_t size) {
    void *tmp;
    size_t allocatedSize;
    tmp=heapPtr;
    allocatedSize=((size-1) / 8 + 1) * 8; /* "worst case" 64-bit alignment assumption */
    heapPtr+=allocatedSize;
    memTotal+=allocatedSize; memCount++;
    printf("cn[]:%lu:%lu:%d:%d:%p:", size, allocatedSize, memTotal, memCount, tmp);
    return tmp;
}

void M2MDynamicMemory::operator delete[] (void * ptr) {
    memCount--; /* still update allocation counter */
    printf("cd[]:nop:%d", memCount);
}
#endif

#ifdef M2M_PASSTHROUGH
/* linux malloc based implementation */
void * M2MDynamicMemory::operator new (size_t size) {
    void *tmp;
#ifdef M2M_TRACE_PRINTS
    long int allocatedSize;
#endif
    tmp=malloc(size);
#ifdef M2M_TRACE_PRINTS
    allocatedSize=malloc_usable_size(tmp);
    memTotal+=allocatedSize; memCount++;
    printf("mn:%lu:%lu:%d:%d:", size, allocatedSize, memTotal, memCount);
#endif
    return tmp;
}

void M2MDynamicMemory::operator delete (void * ptr) {
#ifdef M2M_TRACE_PRINTS
    long int allocatedSize;
    allocatedSize=malloc_usable_size(ptr);
    memCount--;
    memTotal-=allocatedSize;
    printf("md:%lu:%d:%d:", allocatedSize, memTotal, memCount);
#endif
    free(ptr);
}

void * M2MDynamicMemory::operator new[] (size_t size) {
    void *tmp;
#ifdef M2M_TRACE_PRINTS
    long int allocatedSize;
#endif
    tmp=malloc(size);
#ifdef M2M_TRACE_PRINTS
    allocatedSize=malloc_usable_size(tmp);
    memTotal+=allocatedSize; memCount++;
    printf("mn[]:%lu:%lu:%d:%d:", size, allocatedSize, memTotal, memCount);
#endif
    return tmp;
}
void M2MDynamicMemory::operator delete[] (void * ptr) {
#ifdef M2M_TRACE_PRINTS
    long int allocatedSize;
    allocatedSize=malloc_usable_size(ptr);
    memCount--;
    memTotal-=allocatedSize;
    printf("md[]:%lu:%d:%d:", allocatedSize, memTotal, memCount);
#endif
    free(ptr);
}
#endif

void M2MDynamicMemory::init(void) {
    init(M2M_DYNAMIC_MEMORY_HEAP_SIZE);
}

void M2MDynamicMemory::init(size_t heapSize) {
    heap=malloc( heapSize );
#ifdef M2M_TRACE_PRINTS
    printf("Init allocated %lu bytes for cloud client heap at %p\n", heapSize, heap);
#endif
    init(heap, heapSize);
}

void M2MDynamicMemory::init(void *heapAllocation, size_t heapSize) {
    heapSize=heapSize;
    heapPtr=heapAllocation;
#ifdef M2M_DYNMEM_LIB
    m2m_dyn_mem_init((uint8_t *)heapAllocation, (uint16_t)heapSize, &memory_fail_callback, &memInfo);
#endif
}

void M2MDynamicMemory::print_heap_running_statistics() {
#ifdef M2M_TRACE_PRINTS
#ifdef M2M_DYNMEM_LIB
    printf(":%d:%d:", memInfo.heap_sector_allocated_bytes, memInfo.heap_sector_alloc_cnt);
#else
    printf(":%d:%d:", memTotal, memCount);
#endif
#endif
}
void M2MDynamicMemory::print_heap_overall_statistics() {
#ifdef M2M_TRACE_PRINTS
#ifdef M2M_DYNMEM_LIB
    printf(":%d:%d:%u:%u:", memInfo.heap_sector_size,
        memInfo.heap_sector_allocated_bytes_max,
        memInfo.heap_alloc_total_bytes, memInfo.heap_alloc_fail_cnt);
#else
   // printf(":%lu:%lu:%d:%d:", size, allocatedSize, memTotal, memCount);
#endif
#endif
}

void* M2MDynamicMemory::memory_alloc(uint16_t size){
    void *tmp;
    #ifdef M2M_DYNMEM_LIB
    tmp = m2m_dyn_mem_alloc(size);
    #endif
    #ifdef M2M_PASSTHROUGH
    memTotal+=size; memCount++;
    tmp = malloc(size);
    #endif
    #ifdef M2M_TRACE_PRINTS
    printf(":ma:%p", tmp);
    print_heap_running_statistics();
    #endif
    return tmp;
}

void M2MDynamicMemory::memory_free(void *ptr) {
    #ifdef M2M_DYNMEM_LIB
    m2m_dyn_mem_free(ptr);
    #endif
    #ifdef M2M_PASSTHROUGH
    free(ptr);
    #endif
    #ifdef M2M_TRACE_PRINTS
    printf(":mf:%p", ptr);
    print_heap_overall_statistics();
    #endif
 }
