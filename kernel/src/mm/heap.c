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
    heap.heap_addr = (uint64_t*)phys_to_hh((uint64_t)pmm_alloc(FRAZZOS_HEAP_SIZE));
    heap.free_blocks = FRAZZOS_HEAP_SIZE / BLOCK_SIZE;
    heap.used_blocks = 0;
    memset(heap.bitmap, 0, ((FRAZZOS_HEAP_SIZE / BLOCK_SIZE) / BITS_PER_BLOCK) * 8); 
    kprint("Success\n");
}