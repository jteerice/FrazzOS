#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "mm.h"
#include "klibc/memory.h"
#include "klibc/io.h"
#include "klibc/string.h"

struct heap heap = {
    .size = FRAZZOS_HEAP_SIZE
};
extern uint64_t* root_page_dir;

void heap_init() {
    kprint("[KERNEL] Heap Initializing... ");

    heap.addr = pmm_alloc(FRAZZOS_HEAP_SIZE);
    memset(heap.addr, 0, FRAZZOS_HEAP_SIZE);
    memset((void*)heap.map, 0xFF, FRAZZOS_HEAP_SIZE / BITS_PER_BLOCK);
    for (uint64_t i = (uint64_t)heap.addr; i < (uint64_t)heap.addr + align_up(FRAZZOS_HEAP_SIZE); i += PAGE_SIZE) {
        vmm_map_page(root_page_dir, i, FRAZZOS_HEAP_START_ADDR + i, PTE_PRESENT | PTE_READ_WRITE);
    }

    kprint("Success\n");
}