/*
 * Copyright (c) 2014-2015 ARM Limited. All rights reserved.
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
#include <stdint.h>
#include <string.h>
#include "include/m2mdynmemLIB.h"
#include "platform/arm_hal_interrupt.h"
#include <stdlib.h>
#include "ns_list.h"

#ifndef STANDARD_MALLOC
typedef enum mem_stat_update_t {
    DEV_HEAP_ALLOC_OK,
    DEV_HEAP_ALLOC_FAIL,
    DEV_HEAP_FREE,
} mem_stat_update_t;

typedef struct {
    ns_list_link_t link;
} hole_t;

typedef int m2m_word_size_t; // internal signed heap block size type

/* struct for book keeping variables */
typedef struct book {
    m2m_word_size_t     *heap_main;
    m2m_word_size_t     *heap_main_end;
    m2m_mem_stat_t *mem_stat_info_ptr;
    void (*heap_failure_callback)(m2m_heap_fail_t);
    NS_LIST_HEAD(hole_t, link) holes_list;
    size_t heap_size;
} book_t;

// size of a hole_t in our word units
#define HOLE_T_SIZE ((sizeof(hole_t) + sizeof(m2m_word_size_t) - 1) / sizeof(m2m_word_size_t))

static NS_INLINE hole_t *hole_from_block_start(m2m_word_size_t *start)
{
    return (hole_t *)(start + 1);
}

static NS_INLINE m2m_word_size_t *block_start_from_hole(hole_t *start)
{
    return ((m2m_word_size_t *)start) - 1;
}

static void heap_failure(void (*callback)(m2m_heap_fail_t), m2m_heap_fail_t reason)
{
    if (callback) {
        callback(reason);
    }
}

#endif

void m2m_dyn_mem_init(uint8_t *heap, size_t h_size, void (*passed_fptr)(m2m_heap_fail_t), m2m_mem_stat_t *info_ptr)
{
#ifndef STANDARD_MALLOC
    book_t *book = (book_t *)heap;

    book->holes_list.slist.first_entry = NULL;
    book->holes_list.slist.last_nextptr = &(book->holes_list).slist.first_entry;
    m2m_word_size_t *ptr;
    m2m_word_size_t temp_int;
    /* Do memory alignment */
    temp_int = ((uintptr_t)heap % sizeof(m2m_word_size_t));
    if (temp_int) {
        heap += (sizeof(m2m_word_size_t) - temp_int);
        h_size -= (sizeof(m2m_word_size_t) - temp_int);
    }

    /* Make correction for total length also */
    temp_int = (h_size % sizeof(m2m_word_size_t));
    if (temp_int) {
        h_size -= (sizeof(m2m_word_size_t) - temp_int);
    }
    book->heap_main = (m2m_word_size_t *)&(book[1]); // SET Heap Pointer
    book->heap_size = h_size - sizeof(book_t); //Set Heap Size
    temp_int = (book->heap_size / sizeof(m2m_word_size_t));
    temp_int -= 2;
    ptr = book->heap_main;
    *ptr = -(temp_int);
    ptr += (temp_int + 1);
    *ptr = -(temp_int);
    book->heap_main_end = ptr;

    ns_list_init(&book->holes_list);
    ns_list_add_to_start(&book->holes_list, hole_from_block_start(book->heap_main));

    //RESET Memory by Hea Len
    if (info_ptr) {
        book->mem_stat_info_ptr = info_ptr;
        memset(book->mem_stat_info_ptr, 0, sizeof(m2m_mem_stat_t));
        book->mem_stat_info_ptr->heap_sector_size = book->heap_size;
    }
#endif
    //There really is no support to standard malloc in this library anymore
    book->heap_failure_callback = passed_fptr;
}

const m2m_mem_stat_t *m2m_dyn_mem_get_mem_stat(uint8_t *heap)
{
#ifndef STANDARD_MALLOC
    book_t *book = (book_t *)heap;
    return book->mem_stat_info_ptr;
#else
    return NULL;
#endif
}

#ifndef STANDARD_MALLOC
static void dev_stat_update(m2m_mem_stat_t *mem_stat_info_ptr, mem_stat_update_t type, m2m_block_size_t size)
{
    if (mem_stat_info_ptr) {
        switch (type) {
            case DEV_HEAP_ALLOC_OK:
                mem_stat_info_ptr->heap_sector_alloc_cnt++;
                mem_stat_info_ptr->heap_sector_allocated_bytes += size;
                if (mem_stat_info_ptr->heap_sector_allocated_bytes_max < mem_stat_info_ptr->heap_sector_allocated_bytes) {
                    mem_stat_info_ptr->heap_sector_allocated_bytes_max = mem_stat_info_ptr->heap_sector_allocated_bytes;
                }
                mem_stat_info_ptr->heap_alloc_total_bytes += size;
                break;
            case DEV_HEAP_ALLOC_FAIL:
                mem_stat_info_ptr->heap_alloc_fail_cnt++;
                break;
            case DEV_HEAP_FREE:
                mem_stat_info_ptr->heap_sector_alloc_cnt--;
                mem_stat_info_ptr->heap_sector_allocated_bytes -= size;
                break;
        }
    }
}

static m2m_word_size_t convert_allocation_size(book_t *book, m2m_block_size_t requested_bytes)
{
    if (book->heap_main == 0) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_HEAP_SECTOR_UNITIALIZED);
    } else if (requested_bytes < 1) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_ALLOCATE_SIZE_NOT_VALID);
    } else if (requested_bytes > (book->heap_size - 2 * sizeof(m2m_word_size_t)) ) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_ALLOCATE_SIZE_NOT_VALID);
    }
    return (requested_bytes + sizeof(m2m_word_size_t) - 1) / sizeof(m2m_word_size_t);
}

// Checks that block length indicators are valid
// Block has format: Size of data area [1 word] | data area [abs(size) words]| Size of data area [1 word]
// If Size is negative it means area is unallocated
// For direction, use 1 for direction up and -1 for down
static int8_t m2m_block_validate(m2m_word_size_t *block_start, int direction)
{
    int8_t ret_val = -1;
    m2m_word_size_t *end = block_start;
    m2m_word_size_t size_start = *end;
    end += (1 + abs(size_start));
    if (size_start != 0 && size_start == *end) {
        ret_val = 0;
    }
    return ret_val;
}
#endif

// For direction, use 1 for direction up and -1 for down
static void *m2m_dyn_mem_internal_alloc(uint8_t *heap, const m2m_block_size_t alloc_size, int direction)
{
#ifndef STANDARD_MALLOC
    book_t *book = (book_t *)heap;
    m2m_word_size_t *block_ptr = NULL;

    platform_enter_critical();

    m2m_word_size_t data_size = convert_allocation_size(book, alloc_size);
    if (!data_size) {
        goto done;
    }

    // ns_list_foreach, either forwards or backwards, result to ptr
    for (hole_t *cur_hole = direction > 0 ? ns_list_get_first(&book->holes_list)
                                          : ns_list_get_last(&book->holes_list);
         cur_hole;
         cur_hole = direction > 0 ? ns_list_get_next(&book->holes_list, cur_hole)
                                  : ns_list_get_previous(&book->holes_list, cur_hole)
        ) {
        m2m_word_size_t *p = block_start_from_hole(cur_hole);
        if (m2m_block_validate(p, direction) != 0 || *p >= 0) {
            //Validation failed, or this supposed hole has positive (allocated) size
            heap_failure(book->heap_failure_callback, M2M_DYN_MEM_HEAP_SECTOR_CORRUPTED);
            break;
        }
        if (-*p >= data_size) {
            // Found a big enough block
            block_ptr = p;
            break;
        }
    }

    if (!block_ptr) {
        goto done;
    }

    m2m_word_size_t block_data_size = -*block_ptr;
    if (block_data_size >= (data_size + 2 + HOLE_T_SIZE)) {
        m2m_word_size_t hole_size = block_data_size - data_size - 2;
        m2m_word_size_t *hole_ptr;
        //There is enough room for a new hole so create it first
        if ( direction > 0 ) {
            hole_ptr = block_ptr + 1 + data_size + 1;
            // Hole will be left at end of area.
            // Would like to just replace this block_ptr with new descriptor, but
            // they could overlap, so ns_list_replace might fail
            //ns_list_replace(&holes_list, block_ptr, hole_from_block_start(hole_ptr));
            hole_t *before = ns_list_get_previous(&book->holes_list, hole_from_block_start(block_ptr));
            ns_list_remove(&book->holes_list, hole_from_block_start(block_ptr));
            if (before) {
                ns_list_add_after(&book->holes_list, before, hole_from_block_start(hole_ptr));
            } else {
                ns_list_add_to_start(&book->holes_list, hole_from_block_start(hole_ptr));
            }
        } else {
            hole_ptr = block_ptr;
            // Hole remains at start of area - keep existing descriptor in place.
            block_ptr += 1 + hole_size + 1;
        }

        hole_ptr[0] = -hole_size;
        hole_ptr[1 + hole_size] = -hole_size;
    } else {
        // Not enough room for a left-over hole, so use the whole block
        data_size = block_data_size;
        ns_list_remove(&book->holes_list, hole_from_block_start(block_ptr));
    }
    block_ptr[0] = data_size;
    block_ptr[1 + data_size] = data_size;

 done:
    if (book->mem_stat_info_ptr) {
        if (block_ptr) {
            //Update Allocate OK
            dev_stat_update(book->mem_stat_info_ptr, DEV_HEAP_ALLOC_OK, (data_size + 2) * sizeof(m2m_word_size_t));

        } else {
            //Update Allocate Fail, second parameter is not used for stats
            dev_stat_update(book->mem_stat_info_ptr, DEV_HEAP_ALLOC_FAIL, 0);
        }
    }
    platform_exit_critical();

    return block_ptr ? block_ptr + 1 : NULL;
#else
    void *retval = NULL;
    if (alloc_size) {
        platform_enter_critical();
        retval = malloc(alloc_size);
        platform_exit_critical();
    }
    return retval;
#endif
}

void *m2m_dyn_mem_alloc(uint8_t *heap, m2m_block_size_t alloc_size)
{
    return m2m_dyn_mem_internal_alloc(heap, alloc_size, -1);
}

void *m2m_dyn_mem_temporary_alloc(uint8_t *heap, m2m_block_size_t alloc_size)
{
    return m2m_dyn_mem_internal_alloc(heap, alloc_size, 1);
}

#ifndef STANDARD_MALLOC
static void m2m_free_and_merge_with_adjacent_blocks(book_t *book, m2m_word_size_t *cur_block, m2m_word_size_t data_size)
{
    // Theory of operation: Block is always in form | Len | Data | Len |
    // So we need to check length of previous (if current not heap start)
    // and next (if current not heap end) blocks. Negative length means
    // free memory so we can merge freed block with those.

    hole_t *existing_start = NULL;
    hole_t *existing_end = NULL;
    m2m_word_size_t *start = cur_block;
    m2m_word_size_t *end = cur_block + data_size + 1;
    //invalidate current block
    *cur_block = -data_size;
    *end = -data_size;
    m2m_word_size_t merged_data_size = data_size;

    if (cur_block != book->heap_main) {
        cur_block--;
        if (*cur_block < 0) {
            merged_data_size += (2 - *cur_block);
            start -= (2 - *cur_block);
            if (-*start >= HOLE_T_SIZE) {
                existing_start = hole_from_block_start(start);
            }
        }
        cur_block++;
    }

    if (end != book->heap_main_end) {
        end++;
        if (*end < 0) {
            merged_data_size += (2 - *end);
            if (-*end >= HOLE_T_SIZE) {
                existing_end = hole_from_block_start(end);
            }
            end += (1 - *end);
        }else{
            end--;
        }
    }

    hole_t *to_add = hole_from_block_start(start);
    hole_t *before = NULL;
    if (existing_end) {
        // Extending hole described by "existing_end" downwards.
        // Will replace with descriptor at bottom of merged block.
        // (Can't use ns_list_replace, because of danger of overlap)
        // Optimisation - note our position for insertion below.
        before = ns_list_get_next(&book->holes_list, existing_end);
        ns_list_remove(&book->holes_list, existing_end);
    }
    if (existing_start) {
        // Extending hole described by "existing_start" upwards.
        // No need to modify that descriptor - it remains at the bottom
        // of the merged block to describe it.
    } else {
        // Didn't find adjacent descriptors, but may still
        // be merging with small blocks without descriptors.
        if ( merged_data_size >= HOLE_T_SIZE ) {
            // Locate hole position in list, if we don't already know
            // from merging with the block above.
            if (!existing_end) {
                ns_list_foreach(hole_t, ptr, &book->holes_list) {
                    if (ptr > to_add) {
                        before = ptr;
                        break;
                    }
                }
            }
            if (before) {
                ns_list_add_before(&book->holes_list, before, to_add);
            } else {
                ns_list_add_to_end(&book->holes_list, to_add);
            }

        }
    }
    *start = -merged_data_size;
    *end = -merged_data_size;
}
#endif

void m2m_dyn_mem_free(uint8_t *heap, void *block)
{
#ifndef STANDARD_MALLOC
    book_t *book = (book_t *)heap;
    m2m_word_size_t *ptr = block;
    m2m_word_size_t size;

    if (!block) {
        return;
    }

    if (!book->heap_main) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_HEAP_SECTOR_UNITIALIZED);
        return;
    }

    platform_enter_critical();
    ptr --;
    //Read Current Size
    size = *ptr;
    if (size < 0) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_DOUBLE_FREE);
    } else if (ptr < book->heap_main || ptr >= book->heap_main_end) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_POINTER_NOT_VALID);
    } else if ((ptr + size) >= book->heap_main_end) {
        heap_failure(book->heap_failure_callback, M2M_DYN_MEM_POINTER_NOT_VALID);
    } else {
        if (m2m_block_validate(ptr, 1) != 0) {
            heap_failure(book->heap_failure_callback, M2M_DYN_MEM_HEAP_SECTOR_CORRUPTED);
        } else {
            m2m_free_and_merge_with_adjacent_blocks(book, ptr, size);
            if (book->mem_stat_info_ptr) {
                //Update Free Counter
                dev_stat_update(book->mem_stat_info_ptr, DEV_HEAP_FREE, (size + 2) * sizeof(m2m_word_size_t));
            }
        }
    }
    platform_exit_critical();
#else
    platform_enter_critical();
    free(block);
    platform_exit_critical();
#endif
}
