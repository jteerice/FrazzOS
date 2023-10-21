#ifndef PMM_H
#define PMM_H

#define MAX_BUF_LEN 19
#define PAGE_SIZE 0x1000
#define KERNEL_VIRT_ADDRESS_BASE 0xffffffff80000000

#define FREE 0x00
#define USED 0x01

#include <limine.h>

void pmm_init();

#endif