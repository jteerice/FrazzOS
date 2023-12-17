#ifndef SMP_H
#define SMP_H

#include <stdint.h>
#include "mm/mm.h"

#define INIT_IPI_MSG 0x00004500
#define INIT_IPI_DEASSERT 0x00008500
#define SIPI_CODE 0x600

#define TRAMPOLINE_ADDR 0x1000

#define AP_STACK_SIZE 32000

struct ap_stack {
    uint8_t guard_page[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
    uint8_t stack[AP_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
};

void ap_entry();
void smp_init();

#endif