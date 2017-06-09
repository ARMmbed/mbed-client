#include <stdlib.h>
#include "m2mapplicationmemory.h"
void *M2MApplicationMemory::memory_alloc(size_t size) {
    if(size) {
        return malloc(size);
    } else {
        return 0;
    }
}
void M2MApplicationMemory::memory_free(void *ptr) {
    if(ptr) {
        free(ptr);
    }
}
