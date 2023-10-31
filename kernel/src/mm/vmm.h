#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define PTE_PRESENT 0x01
#define PTE_READ_WRITE 0x02
#define PTE_USER_SUPERVISOR 0x04
#define PTE_WRITE_THROUGH 0x08
#define PTE_CACHE_DISABLE 0x10
#define PTE_ACCESSED 0x20
#define PTE_DIRTY 0x40
#define PTE_PAT 0x80
#define PTE_GLOBAL 0x100

#define GIGABYTE 0x40000000UL
#define INDEX_MASK 511

void vmm_map_page(uint64_t* root_page_dir, uint64_t phys_addr, uint64_t virt_addr, int flags);
void vmm_unmap_page(uint64_t* root_page_dir, uint64_t virt_addr);
void vmm_init();

#endif