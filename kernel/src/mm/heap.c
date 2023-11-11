#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "mm.h"
#include "klibc/memory.h"
#include "klibc/io.h"
#include "klibc/alloc.h"
#include "klibc/string.h"

struct heap heap;

void heap_init() {
    kprint("[KERNEL] Heap Initializing... ");
    heap.heap_addr = phys_to_hh(pmm_alloc(FRAZZOS_HEAP_SIZE));
    heap.free_blocks = FRAZZOS_HEAP_SIZE / BLOCK_SIZE;
    heap.used_blocks = 0;
    memset(heap.bitmap, 0, ((FRAZZOS_HEAP_SIZE / BLOCK_SIZE) / BITS_PER_BLOCK) * 8); 
    kprint("Success\n");
}

/*struct heap heap = {
    .size = FRAZZOS_HEAP_SIZE
};
extern uint64_t* root_page_dir;

void heap_init() {
    kprint("[KERNEL] Heap Initializing... ");

    heap.addr = (uint64_t*)mmap((uintptr_t)(4 * GIGABYTE), FRAZZOS_HEAP_SIZE, PTE_PRESENT | PTE_READ_WRITE);
    memset(heap.addr, 0, FRAZZOS_HEAP_SIZE);
    memset((void*)heap.map, 0xFF, FRAZZOS_HEAP_SIZE / BITS_PER_BLOCK);

    kprint("Success\n");
}*/

