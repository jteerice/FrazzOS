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
