#include "pmm.h"
#include "config.h"
#include "klibc/string.h"
#include "klibc/io.h"

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
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
        if (entries[i]->type == LIMINE_MEMMAP_USABLE) {
            kprint("1 (Available)\n");
        } else {
            kprint("2 (Reserved)\n");
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
    char num_buf[MAX_NUM_STRING_LEN];
    print_mem_map(num_entries, entries);
    kprint("\n");
    kprint("Total Amount of Memory: 0xFFFFFFFFFFFFFFFF\n");
    kprint("Total 4KB Blocks: ");
    kprint(itoa(num_buf, (0xFFFFFFFFFFFFFFFF / PAGE_SIZE)));
    kprint("\nTotal Reserved Blocks: ");
    kprint(itoa(num_buf, (get_total_memory(num_entries, entries, 1) / PAGE_SIZE)));
    kprint("\nTotal Available Blocks: ");
    kprint(itoa(num_buf, (get_total_memory(num_entries, entries, 0) / PAGE_SIZE)));
    kprint("\n\n");
    print_kernel_base_addresses();
    kprint("\n");
}

void pmm_init() {
    int num_entries = memmap_request.response->entry_count;
    struct limine_memmap_entry** entries = memmap_request.response->entries;

    print_memory_stats(num_entries, entries);

    //uint64_t detected_memory = get_total_memory(num_entries, entries, 0);
    //uint64_t bitmap_size = ((detected_memory / PAGE_SIZE) / 8);
}