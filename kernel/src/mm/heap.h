#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include "vmm.h"

#define FRAZZOS_HEAP_SIZE 104857600
#define FRAZZOS_HEAP_START_ADDR (4 * GIGABYTE)

struct heap {
    uint64_t* addr;
    uint64_t size;
    uint64_t available_bytes;
    uint64_t map[FRAZZOS_HEAP_SIZE / 64];
};

void heap_init();

#endif