#ifndef PMM_H
#define PMM_H

#define MAX_BUF_LEN 19

#define FREE 0x00
#define USED 0x01

#include <limine.h>
#include <stdbool.h>

struct pmm_info {
    uint64_t total_memory;
    uint64_t total_available_memory;
    uint64_t total_reserved_memory;
    uint64_t total_pages;
    uint64_t used_pages;
    uint64_t available_pages;
};

struct bitmap {
    uint64_t* map;
    uint64_t size;
}__attribute((packed));

void pmm_init();
void* pmm_alloc(uint64_t size);

#endif