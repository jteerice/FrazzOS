#include "pmm.h"
#include "status.h"
#include "mm.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "klibc/memory.h"

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
    struct limine_kernel_address_response* response = kernel_address_request.response;
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

static inline void bitmap_clear_bit(uint64_t idx) {
    bitmap.map[idx / BITS_PER_BLOCK] &= ~(1 << (idx % BITS_PER_BLOCK));
}

static void pmm_free(uint64_t addr, uint64_t len) {
    for (uint64_t i = addr / PAGE_SIZE; i <  ((addr + len) / PAGE_SIZE) + (len % PAGE_SIZE ? 1 : 0); i++) {
        bitmap_clear_bit(i);
    }
}

static inline void bitmap_set_bit(uint64_t idx) {
    bitmap.map[idx / BITS_PER_BLOCK] |= (1 << (idx % BITS_PER_BLOCK));
}

static void pmm_reserve(uint64_t addr, uint64_t len) {
    for (uint64_t i = addr / PAGE_SIZE; i <  ((addr + len) / PAGE_SIZE) + (len % PAGE_SIZE ? 1 : 0); i++) {
        bitmap_set_bit(i);
    }
}

static inline bool bitmap_test_bit(uint64_t idx) {
    return bitmap.map[idx / BITS_PER_BLOCK] & (1 << (idx % BITS_PER_BLOCK));
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
                //entries[i]->base += bitmap.size;
                //entries[i]->length -= bitmap.size;
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