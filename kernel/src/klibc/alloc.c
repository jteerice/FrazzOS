#include "alloc.h"
#include "mm/mm.h"
#include "mm/heap.h"
#include "status.h"
#include "bitmap.h"
#include "string.h"
#include "mm/vmm.h"
#include "mm/pmm.h"
#include "klibc/io.h"

extern struct heap heap;
extern uint64_t* root_page_dir;

void* malloc(uint64_t size) {
    if (heap.free_blocks < (size / BLOCK_SIZE)) {
        return (void*)ENOMEM;
    }

    int next_start;
    uint64_t buffer_long_size = ((FRAZZOS_HEAP_SIZE / BLOCK_SIZE) / BITS_PER_BLOCK) * 8;
    int start = find_first_free(0, heap.bitmap, buffer_long_size);

    do {
        next_start = is_free_region_big_enough(start, heap.bitmap, size / BLOCK_SIZE);
        if (next_start != 0) {
            start = find_first_free(next_start, heap.bitmap, buffer_long_size);
        } else {
            break;
        }
    } while (start >= 0);

    if (start == -1) {
        return (void*)ENOMEM;
    }

    for (uint64_t i = start; i < (uint64_t)(start + (size / BLOCK_SIZE)); i++) {
        bitmap_set_bit(i, heap.bitmap);
    }

    heap.free_blocks -= size / BLOCK_SIZE;
    heap.used_blocks += size / BLOCK_SIZE;

    return (void*)(heap.heap_addr + (start * BLOCK_SIZE));
}

void* mmap(uintptr_t addr, uint64_t size, int flags) {
    if (size % PAGE_SIZE != 0) {
        size = align_up(size); 
    }
    void* ptr = pmm_alloc(size);
    uint64_t pages = size / PAGE_SIZE;
    for (uint64_t i = 0; i < pages; i += PAGE_SIZE) {
        vmm_map_page(root_page_dir, (uint64_t)(ptr + i), (uint64_t)(addr + i), flags);
    }
    return (void*)addr;
}