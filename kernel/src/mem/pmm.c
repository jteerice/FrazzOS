#include "pmm.h"
#include "klibc/string.h"
#include "klibc/io.h"

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static void print_mem_map(int num_entries, struct limine_memmap_entry** entries) {
    char hex_addr[HEX_STRING_MAX];
    for (int i = 0; i < num_entries; i++) {
        if (entries[i]->type == LIMINE_MEMMAP_USABLE) {
            kprint("Usable memory starting at: ");
            kprint(ull_to_hex(hex_addr, entries[i]->base));
            kprint(" to "); 
            kprint(ull_to_hex(hex_addr, (entries[i]->base + entries[i]->length)));
            kprint(" for ");
            kprint(ull_to_hex(hex_addr, entries[i]->length));
            kprint(" bytes\n");
        }
    }
}

void pmm_init() {
    int num_entries = memmap_request.response->entry_count;
    struct limine_memmap_entry** entries = memmap_request.response->entries;

    print_mem_map(num_entries, entries);    


}