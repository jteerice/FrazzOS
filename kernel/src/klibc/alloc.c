#include "alloc.h"
#include "mm/mm.h"
#include "mm/heap.h"
#include "status.h"

extern struct heap heap;

int first_free_byte(int start) {
    for (uint64_t i = start; i < heap.size / BITS_PER_BLOCK; i++) {
        if (heap.map[i] != 0xFFFFFFFFFFFFFFFF) {
            for (int j = 0; j < BITS_PER_BLOCK; j++) {
                if (!(heap.map[i] & (1 << j))) {
                    return i * BITS_PER_BLOCK + j;
                }
            }
        }
    }
    return -1;
}

void* malloc(uint64_t size) {
    int start = first_free_byte(0);
    while (start >= 0) {
        if (is_free_region_big_enough(start, size)) {
            start = first_free_byte(start);
        }
    }
}