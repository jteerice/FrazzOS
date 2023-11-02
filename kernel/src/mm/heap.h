#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

#define FRAZZOS_HEAP_SIZE 104857600
#define FRAZZOS_HEAP_START_ADDR (4 * GIGABYTE)

struct heap {
    uint64_t* addr;
    uint64_t size;
    volatile uint64_t map[FRAZZOS_HEAP_SIZE / 64];
}__attribute__((packed));

void heap_init();

#endif