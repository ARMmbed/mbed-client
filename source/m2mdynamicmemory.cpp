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

//#include <new>
#include <malloc.h>
#include "include/m2mdynamicmemory.h"
#include "include/m2mdynmemLIB.h"

int M2MDynamicMemory::memTotal = 0;
int M2MDynamicMemory::memCount = 0;
void * M2MDynamicMemory::heap = 0;
void * M2MDynamicMemory::heapPtr = 0;
int M2MDynamicMemory::referenceCount = 0;
mem_stat_t /*M2MDynamicMemory::*/memInfo;

//#define M2M_DYNMEM_LIB
#define M2M_PASSTHROUGH
#define M2M_TRACE_PRINTS

#ifdef M2M_DYNMEM_LIB

/* nanostack dynmemlib based implementation */
void * M2MDynamicMemory::operator new (size_t size) {
    void *tmp;
    size_t allocatedSize;
    //if (size == 0) return 0;
    tmp=m2m_dyn_mem_alloc(size);
//	tmp=malloc(size);
//	allocatedSize=((size-1) / 8 + 1) * 8; /* "worst case" 64-bit alignment assumption */
//	heapPtr+=allocatedSize;
    memTotal+=size; memCount++;
//    printf("cn:%lu:%lu:%d:%d:", size, /*allocatedSize*/ (long int)0, memTotal, memCount);
#ifdef M2M_TRACE_PRINTS
    printf("mn"); /* M2M new */
    print_heap_running_statistics();
    //print_heap_overall_statistics();
#endif
    return tmp;
}

void M2MDynamicMemory::operator delete (void * ptr) {
    memCount--; /* still update allocation counter */
    m2m_dyn_mem_free(ptr);
//    printf("cd:%d", memCount);
#ifdef M2M_TRACE_PRINTS
    printf("md"); /* M2M delete */
    print_heap_overall_statistics();
#endif
}
void * M2MDynamicMemory::operator new[] (size_t size) {
    void *tmp;
    tmp=m2m_dyn_mem_alloc(size);
    memTotal+=size; memCount++;
    //printf("cn[]:%lu:%d:%d:", size, memTotal, memCount);
#ifdef M2M_TRACE_PRINTS
    printf("mn[]"); /* M2M new array */
#endif
    return tmp;
}
void M2MDynamicMemory::operator delete[] (void * ptr) {
    memCount--;
    m2m_dyn_mem_free(ptr);
#ifdef M2M_TRACE_PRINTS
    printf("md[]"); /* M2M delete array */
#endif
//  printf("cd[]:%d:", memCount);
}
#endif
#if 0
/* own very simple only alloc alloc */
void * M2MDynamicMemory::operator new (size_t size) {
    void *tmp;
    size_t allocatedSize;
    //printf("(pcn)");
    //if (size == 0) return 0;
    tmp=heapPtr;
    allocatedSize=((size-1) / 8 + 1) * 8; /* "worst case" 64-bit alignment assumption */
    heapPtr+=allocatedSize;
    memTotal+=allocatedSize; memCount++;
    printf("cn:%lu:%lu:%d:%d:%p:", size, allocatedSize, memTotal, memCount, tmp);
    //throw std::bad_alloc();
    return tmp;
}

void M2MDynamicMemory::operator delete (void * ptr) {
    memCount--; /* still update allocation counter */
    printf("cd:nop:%d", memCount);
}

void * M2MDynamicMemory::operator new[] (size_t size) {
    void *tmp;
    size_t allocatedSize;
    //if (size == 0) return 0;
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
    long int allocatedSize;
    tmp=malloc(size);
    allocatedSize=malloc_usable_size(tmp);
    memTotal+=allocatedSize; memCount++;
    printf("cn:%lu:%lu:%d:%d:", size, allocatedSize, memTotal, memCount);
    return tmp;
}

void M2MDynamicMemory::operator delete (void * ptr) {
    long int allocatedSize;
    allocatedSize=malloc_usable_size(ptr);
    memCount--;
    memTotal-=allocatedSize;
    printf("cd:%lu:%d:%d:", allocatedSize, memTotal, memCount);
    free(ptr);
}

void * M2MDynamicMemory::operator new[] (size_t size) {
    void *tmp;
    long int allocatedSize;
    tmp=malloc(size);
    allocatedSize=malloc_usable_size(tmp);
    memTotal+=allocatedSize; memCount++;
    printf("cn[]:%lu:%lu:%d:%d:", size, allocatedSize, memTotal, memCount);
    return tmp;
}
void M2MDynamicMemory::operator delete[] (void * ptr) {
    long int allocatedSize;
    allocatedSize=malloc_usable_size(ptr);
    memCount--;
    memTotal-=allocatedSize;
    printf("cd[]:%lu:%d:%d:", allocatedSize, memTotal, memCount);
    free(ptr);
}
#endif

void M2MDynamicMemory::init(void) {
//    printf("Init allocated %d bytes for cloud client heap\n",
//        );
//    heap=malloc( M2M_DYNAMIC_MEMORY_HEAP_SIZE);
//    heapPtr=heap;
//    m2m_dyn_mem_init((uint8_t *)heap, M2M_DYNAMIC_MEMORY_HEAP_SIZE, 0, &memInfo);
    init(M2M_DYNAMIC_MEMORY_HEAP_SIZE);
}

void M2MDynamicMemory::init(size_t heapSize) {
    heap=malloc( heapSize );
    printf("Init allocated %lu bytes for cloud client heap at %p\n", heapSize, heap);
//    heapPtr=heap;
//    m2m_dyn_mem_init((uint8_t *)heap, (uint16_t)heapSize, 0, &memInfo);
    init(heap, heapSize);
}

void M2MDynamicMemory::init(void *heapAllocation, size_t heapSize) {
//    heap=malloc( M2M_DYNAMIC_MEMORY_HEAP_SIZE)i;
  //  printf("Init allocated %d bytes for cloud client heap at %p\n", heapSize, heap);
    heapSize=heapSize;
    heapPtr=heapAllocation;
#ifdef M2M_DYNMEM_LIB
    m2m_dyn_mem_init((uint8_t *)heapAllocation, (uint16_t)heapSize, 0, &memInfo);
#endif
}

void M2MDynamicMemory::print_heap_running_statistics() {
#ifdef M2M_DYNMEM_LIB
    printf(":%d:%d:", memInfo.heap_sector_allocated_bytes, memInfo.heap_sector_alloc_cnt);
#else
    printf(":%d:%d:", memTotal, memCount);
#endif
}
void M2MDynamicMemory::print_heap_overall_statistics() {
#ifdef M2M_DYNMEM_LIB
    printf(":%d:%d:%u:%u:", memInfo.heap_sector_size,
        memInfo.heap_sector_allocated_bytes_max,
        memInfo.heap_alloc_total_bytes, memInfo.heap_alloc_fail_cnt);
#else
   // printf(":%lu:%lu:%d:%d:", size, allocatedSize, memTotal, memCount);
#endif
}
M2MDynamicMemory::M2MDynamicMemory(void) {
#if 0
    if (heap == 0) {
        printf("Allocated %d bytes for cloud client heap\n",
            CLOUD_CLIENT_HEAP_SIZE);
        heap=malloc(CLOUD_CLIENT_HEAP_SIZE);
        heapPtr=heap;
        cloud_client_dyn_mem_init((uint8_t *)heap, CLOUD_CLIENT_HEAP_SIZE, 0, &memInfo);
    }
    referenceCount++;
    //printf ("debug heapPtr: %p, ref: %d\n", heap, referenceCount);
#endif
}
M2MDynamicMemory::~M2MDynamicMemory(void) {
#if 0
    referenceCount--;
    printf("Cloud client heap destructor, refCount: %d\n", referenceCount);
    if (heap && !referenceCount) {
        printf("Deleting cloud client heap\n");
        free(heap);
        heap=0;
    }
#endif
}
    /**
     * \brief Memory allocation required for libCoap.
     * \param size The size of memory to be reserved.
    */
void* M2MDynamicMemory::memory_alloc(short int size){
    #ifdef M2M_TRACE_PRINTS
    printf("ma");
    print_heap_running_statistics();
    #endif
    #ifdef M2M_DYNMEM_LIB
    return m2m_dyn_mem_alloc(size);
    #endif
    #ifdef M2M_PASSTHROUGH
    memTotal+=size; memCount++;
    return malloc(size);
    #endif

}

    /**
     * \brief Memory free functions required for libCoap.
     * \param ptr The object whose memory needs to be freed.
    */
void M2MDynamicMemory::memory_free(void *ptr) {
    #ifdef M2M_TRACE_PRINTS
    printf("mf");
    print_heap_overall_statistics();
    #endif
    #ifdef M2M_DYNMEM_LIB
    m2m_dyn_mem_free(ptr);
    #endif
    #ifdef M2M_PASSTHROUGH
    free(ptr);
    #endif
}
