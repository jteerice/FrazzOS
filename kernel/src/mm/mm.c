#include "mm.h"

uint64_t align_up(uint64_t addr) {
    if (addr % PAGE_SIZE == 0) {
        return addr;
    }

    return addr + (PAGE_SIZE - (addr % PAGE_SIZE));
}

uint64_t align_down(uint64_t addr) {
    if (addr % PAGE_SIZE == 0) {
        return addr;
    }

    return addr - (addr % PAGE_SIZE);
}

inline uint64_t phys_to_hh(uint64_t addr) {
    return addr + KERNEL_VIRT_BASE_ADDR;
}

inline uint64_t hh_to_phys(uint64_t addr) {
    return addr - KERNEL_VIRT_BASE_ADDR;
}