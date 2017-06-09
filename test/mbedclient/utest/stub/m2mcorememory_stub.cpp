#include <stdlib.h>
#include "m2mcorememory.h"
void *M2MCoreMemory::memory_alloc(size_t size) {
    if(size) {
        return malloc(size);
    } else {
        return 0;
    }
}
void M2MCoreMemory::memory_free(void *ptr) {
    if(ptr) {
        free(ptr);
    }
}
