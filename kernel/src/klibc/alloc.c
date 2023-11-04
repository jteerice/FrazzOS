#include "alloc.h"
#include "mm/mm.h"
#include "mm/heap.h"
#include "status.h"
#include "bitmap.h"

extern struct heap heap;

void* malloc(uint64_t size) {
    uint64_t buffer_long_size = heap.size / BYTES_PER_LONG;
    int start = find_first_free(0, heap.map, buffer_long_size);

    do {
        if (is_free_region_big_enough(start, heap.map, size)) {
            start = find_first_free(start, heap.map, buffer_long_size);
        } else {
            break;
        }
    } while (start >= 0);

    if (start == -1) {
        return (void*)ENOMEM;
    }

    for (uint64_t i = start; i < (uint64_t)(start + (size / BITS_PER_BLOCK)); i++) {
        bitmap_set_bit(i, heap.map);
    }

    heap.available_bytes -= size;

    return (void*)(FRAZZOS_HEAP_START_ADDR + start);
}