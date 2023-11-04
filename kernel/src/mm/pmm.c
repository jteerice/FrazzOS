#include "pmm.h"
#include "status.h"
#include "mm.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "klibc/memory.h"
#include "klibc/bitmap.h"

volatile struct bitmap bitmap;
struct pmm_info pmm_info;

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

struct limine_kernel_address_response* response;

static void print_mem_map(int num_entries, struct limine_memmap_entry** entries) {
    char hex_addr[HEX_STRING_MAX];
    kprint("\nPhysical Memory Regions:\n\n");
    for (int i = 0; i < num_entries; i++) {
        kprint("Memory region base: ");
        kprint(ull_to_hex(hex_addr, entries[i]->base));
        kprint(" -- Length: ");
        kprint(ull_to_hex(hex_addr, entries[i]->length));
        kprint(" -- Type: ");
        switch (entries[i]->type) {
            case LIMINE_MEMMAP_USABLE:
                kprint("0 (Usable)\n"); 
                break;
            case LIMINE_MEMMAP_RESERVED:
                kprint("1 (Reserved)\n");
                break;
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                kprint("2 (ACPI Reclaimable)\n");
                break;
            case LIMINE_MEMMAP_ACPI_NVS:
                kprint("3 (ACPI NVS)\n");
                break;
            case LIMINE_MEMMAP_BAD_MEMORY:
                kprint("4 (Bad Memory)\n");
                break;
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                kprint("5 (Bootloader Reclaimable)\n");
                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                kprint("6 (Kernel and Modules)\n");
                break;
            case LIMINE_MEMMAP_FRAMEBUFFER:
                kprint("7 (Framebuffer)\n");
                break;
        }
    }
}

static uint64_t get_total_memory(int num_entries, struct limine_memmap_entry** entries, int type) {
    uint64_t res = 0;
    if (type == 0) {
        for (int i = 0; i < num_entries; i++) {
            if (entries[i]->type == LIMINE_MEMMAP_USABLE) {
                res += entries[i]->length;
            }
        }
    } else {
        for (int i = 0; i < num_entries; i++) {
            if (entries[i]->type != LIMINE_MEMMAP_USABLE) {
                res += entries[i]->length;
            }
        }
    }

    return res;
}

static void print_kernel_base_addresses() {
    char hex_addr[HEX_STRING_MAX];
    response = kernel_address_request.response;
    kprint("The physical base address of the kernel is: ");
    kprint(ull_to_hex(hex_addr, response->physical_base));
    kprint("\nThe virtual base address of the kernel is:  ");
    kprint(ull_to_hex(hex_addr, response->virtual_base));
}

static void print_memory_stats(int num_entries, struct limine_memmap_entry** entries) {
    pmm_info.total_memory = get_total_memory(num_entries, entries, 0) + get_total_memory(num_entries, entries, 1);
    pmm_info.total_pages = (pmm_info.total_memory / PAGE_SIZE);
    pmm_info.total_available_memory = get_total_memory(num_entries, entries, 0);
    pmm_info.total_reserved_memory = get_total_memory(num_entries, entries, 1);
    pmm_info.used_pages = pmm_info.total_reserved_memory / PAGE_SIZE;
    pmm_info.available_pages = pmm_info.total_available_memory / PAGE_SIZE;
    char num_buf[MAX_NUM_STRING_LEN];
    print_mem_map(num_entries, entries);
    kprint("\n");
    kprint("Total Amount of Memory: ");
    kprint(itoa(num_buf, pmm_info.total_memory));
    kprint("\nTotal 4KB Pages: ");
    kprint(itoa(num_buf, pmm_info.total_pages));
    kprint("\nTotal Reserved Pages: ");
    kprint(itoa(num_buf, pmm_info.used_pages));
    kprint("\nTotal Available Pages: ");
    kprint(itoa(num_buf, pmm_info.available_pages));
    kprint("\n\n");
    print_kernel_base_addresses();
    kprint("\nHigher half base offset: ");
    kprint(ull_to_hex(num_buf, hhdm_request.response[0].offset));
    kprint("\n");
}

static inline void pmm_bitmap_clear_bit(uint64_t idx) {
    bitmap.map[idx / BITS_PER_BLOCK] &= ~(1 << (idx % BITS_PER_BLOCK));
}

static void pmm_free(uint64_t addr, uint64_t len) {
    for (uint64_t i = addr / PAGE_SIZE; i <  ((addr + len) / PAGE_SIZE) + (len % PAGE_SIZE ? 1 : 0); i++) {
        pmm_bitmap_clear_bit(i);
    }
}

static inline void pmm_bitmap_set_bit(uint64_t idx) {
    bitmap.map[idx / BITS_PER_BLOCK] |= (1 << (idx % BITS_PER_BLOCK));
}

static void pmm_reserve(uint64_t addr, uint64_t len) {
    for (uint64_t i = addr / PAGE_SIZE; i <  ((addr + len) / PAGE_SIZE) + (len % PAGE_SIZE ? 1 : 0); i++) {
        pmm_bitmap_set_bit(i);
    }
}

static inline bool pmm_bitmap_test_bit(uint64_t idx) {
    return bitmap.map[idx / BITS_PER_BLOCK] & (1 << (idx % BITS_PER_BLOCK));
}

static int pmm_first_free(uint64_t start) {
    int start_block = start / BITS_PER_BLOCK;
    if (start % BITS_PER_BLOCK != 0) {
        int start_bit = start % BITS_PER_BLOCK;
        for (int i = start_bit; i < BITS_PER_BLOCK; i++) {
            if (!(bitmap.map[start_block] & (1 << i))) {
                return start_block * BITS_PER_BLOCK + i; 
            }
        } 
        start_block++;
    }
    for (uint64_t i = start_block; i < pmm_info.total_pages / BITS_PER_BLOCK; i++) {
        if (bitmap.map[i] != 0xFFFFFFFFFFFFFFFF) {
            for (int j = 0; j < BITS_PER_BLOCK; j++) {
                if (!(bitmap.map[i] & (1 << j))) {
                    return i * BITS_PER_BLOCK + j;
                }
            }
        }
    }
    return ENOMEM;
}

void pmm_free_page(void* addr) {
    pmm_bitmap_clear_bit((uint64_t)addr / PAGE_SIZE);
    pmm_info.available_pages++;
    pmm_info.used_pages--;
}

static int check_region_size(uint64_t start, int count) {
    for (uint64_t i = 0; i < (uint64_t)count; i++) {
        if (pmm_bitmap_test_bit(start + i)) {
            return start + i;
        }
    }
    return 0;
}

void* pmm_alloc(uint64_t size) {
    if (pmm_info.available_pages <= 0) {
        return (void*)ENOMEM;
    }

    int start_idx;
    int num_pages = align_up(size) / PAGE_SIZE;
    int64_t page_frame = pmm_first_free(0);

    do {
        start_idx = check_region_size(page_frame, num_pages);
        if (start_idx != 0) {
            page_frame = pmm_first_free(start_idx);
        } else {
            break;
        }
    } while (page_frame >= 0);

    if ((int)page_frame == -1) {
        return (void*)ENOMEM;
    }

    for (uint64_t i = page_frame; i < ((uint64_t)num_pages + page_frame); i++) {
        pmm_bitmap_set_bit(i);
    }

    pmm_info.available_pages -= num_pages;
    pmm_info.used_pages += num_pages;

    return (void*)(uint64_t)(phys_to_hh(page_frame * PAGE_SIZE));
}

void pmm_init() {
    int num_entries = memmap_request.response->entry_count;
    struct limine_memmap_entry** entries = memmap_request.response->entries;
    print_memory_stats(num_entries, entries);

    bitmap.size = (pmm_info.total_memory / PAGE_SIZE) / BITS_PER_BLOCK;

    for (int i = 0; i < num_entries; i++) {
        if (entries[i]->type == LIMINE_MEMMAP_USABLE) {
            if (entries[i]->length >= bitmap.size) {
                bitmap.map = (uint64_t*)phys_to_hh(entries[i]->base);
            }
        }
    }

    memset(bitmap.map, 0xFF, bitmap.size * 8);

    for (int i = 0; i < num_entries; i++) {
        if (entries[i]->type == LIMINE_MEMMAP_USABLE) {
            pmm_free(entries[i]->base, entries[i]->length);
        }
    }

    pmm_reserve(*bitmap.map, bitmap.size * 8);
    pmm_reserve(0, 0x100000);

    kprint("\n[KERNEL] PMM Initialized... Success\n");
}