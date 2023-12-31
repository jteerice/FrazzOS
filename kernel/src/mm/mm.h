#ifndef MM_H
#define MM_H

#include <stdint.h>

#define PAGE_SIZE 0x1000
#define KERNEL_VIRT_BASE_ADDR 0xffff800000000000
#define KERNEL_VIRT_TOP_ADDR 0xffffffff80000000
#define BITS_PER_BLOCK 64

uint64_t align_up(uint64_t addr);
uint64_t align_down(uint64_t addr);

static inline uintptr_t phys_to_hh(uintptr_t addr) {
    return addr + KERNEL_VIRT_BASE_ADDR;
}

static inline uintptr_t hh_to_phys(uintptr_t addr) {
    return addr - KERNEL_VIRT_BASE_ADDR;
}

static inline uintptr_t phys_to_hh_code(uintptr_t addr) {
    return addr + KERNEL_VIRT_TOP_ADDR;
}

#endif