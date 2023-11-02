#include "vmm.h"
#include "pmm.h"
#include "mm.h"
#include "klibc/memory.h"
#include "klibc/io.h"
#include "klibc/string.h"

extern struct limine_kernel_address_response* response;

uint64_t *root_page_dir;

static uint64_t *vmm_get_next_map_level(uint64_t *page_directory, uintptr_t index, int flags) {
    if (page_directory[index] & 1) {
        return (uint64_t *)(page_directory[index] & ~(INDEX_MASK));
    } else {
        page_directory[index] = hh_to_phys((uint64_t)pmm_alloc(PAGE_SIZE)) | flags;
        return (uint64_t *)(page_directory[index] & ~(INDEX_MASK));
    }
}

void vmm_flush_tlb(void *address) {
    asm volatile("invlpg (%0)" : : "r" (address));
}

void vmm_map_page(uint64_t *root_page_dir, uintptr_t phys_addr, uintptr_t virt_addr, int flags) {
    uintptr_t idx_4 = (virt_addr & ((uint64_t)INDEX_MASK << 39)) >> 39;
    uintptr_t idx_3 = (virt_addr & ((uint64_t)INDEX_MASK << 30)) >> 30;
    uintptr_t idx_2 = (virt_addr & ((uint64_t)INDEX_MASK << 21)) >> 21;
    uintptr_t idx_1 = (virt_addr & ((uint64_t)INDEX_MASK << 12)) >> 12;

    uint64_t *level_4 = root_page_dir;
    uint64_t *level_3 = NULL;
    uint64_t *level_2 = NULL;
    uint64_t *level_1 = NULL;

    level_3 = vmm_get_next_map_level(level_4, idx_4, flags);
    level_2 = vmm_get_next_map_level(level_3, idx_3, flags);
    level_1 = vmm_get_next_map_level(level_2, idx_2, flags);
    
    level_1[idx_1] = phys_addr | flags;

    vmm_flush_tlb((void*)virt_addr);
}

void vmm_unmap_page(uint64_t *root_page_dir, uintptr_t virt_addr) {
    uintptr_t idx_4 = (virt_addr & ((uint64_t)INDEX_MASK << 39)) >> 39;
    uintptr_t idx_3 = (virt_addr & ((uint64_t)INDEX_MASK << 30)) >> 30;
    uintptr_t idx_2 = (virt_addr & ((uint64_t)INDEX_MASK << 21)) >> 21;
    uintptr_t idx_1 = (virt_addr & ((uint64_t)INDEX_MASK << 12)) >> 12;

    uint64_t *level_4 = root_page_dir;
    uint64_t *level_3 = NULL;
    uint64_t *level_2 = NULL;
    uint64_t *level_1 = NULL;

    level_3 = vmm_get_next_map_level(level_4, idx_4, 0);
    level_2 = vmm_get_next_map_level(level_3, idx_3, 0);
    level_1 = vmm_get_next_map_level(level_2, idx_2, 0);

    level_1[idx_1] = 0;

    vmm_flush_tlb((void*)virt_addr);
}

uint64_t *vmm_new_page_dir() {
    uint64_t *new_page_dir = pmm_alloc(PAGE_SIZE);
    memset((void*)hh_to_phys((uint64_t)new_page_dir), 0, PAGE_SIZE);
    return new_page_dir;
}

void vmm_activate_page_directory(uint64_t *current_page_directory) {
    asm volatile("mov %0, %%cr3" : : "r" (hh_to_phys((uint64_t)current_page_directory)) : "memory");
}

void vmm_init() {
    root_page_dir = vmm_new_page_dir();

    // Identity map first 4 GB
    for (uint64_t i = 0x1000; i < 4 * GIGABYTE; i += PAGE_SIZE) {
        vmm_map_page(root_page_dir, i, i, PTE_PRESENT | PTE_READ_WRITE);
    } 

    kprint("[KERNEL] 1/3 Identity Mapped First 4GB\n");

    // Direct map
    for (uint64_t i = 0; i < 4 * GIGABYTE; i += PAGE_SIZE) {
        vmm_map_page(root_page_dir, i, phys_to_hh(i), PTE_PRESENT | PTE_READ_WRITE);
    } 

    kprint("[KERNEL] 2/3 Direct Map First 4GB to 0xffff800000000000\n");

    // Map kernel code
    for (uint64_t i = 0; i < 4 * GIGABYTE; i += PAGE_SIZE) {
        vmm_map_page(root_page_dir, response->physical_base + i, KERNEL_VIRT_TOP_ADDR + i, PTE_PRESENT | PTE_READ_WRITE); 
    }

    kprint("[KERNEL] 3/3 Kernel Code Mapped\n");

    vmm_activate_page_directory(root_page_dir); 

    kprint("[KERNEL] VMM Initialized... Success\n");
}