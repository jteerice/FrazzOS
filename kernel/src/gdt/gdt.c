#include "gdt.h"
#include "../config.h"
#include "../memory/memory.h"
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;

struct gdt gdt[FRAZZOS_TOTAL_SEGMENTS];
struct gdt_structured gdt_structured[FRAZZOS_TOTAL_SEGMENTS] = {
    { .access_byte = 0x00, .flags = 0x00 }, // Null Descriptor
    { .access_byte = 0x9A, .flags = 0xA  }, // Kernel Code
    { .access_byte = 0x92, .flags = 0xC  }, // Kernel Data
    { .access_byte = 0xFA, .flags = 0xA  }, // User Code
    { .access_byte = 0xF2, .flags = 0xC  }, // User Data
    { .access_byte = 0x89, .flags = 0x00 }  // TSS (Need to implement)
};

void encode_gdt_entry(uint8_t* target, struct gdt_structured source) {
    target[5] = source.access_byte;
    target[6] |= (source.flags << 4);
}

void gdt_init() {
    memset(gdt, 0, sizeof(gdt));
    for (size_t i = 0; i < FRAZZOS_TOTAL_SEGMENTS - 1; i++) {
        encode_gdt_entry((uint8_t*)&gdt[i], gdt_structured[i]);
    }
    load_gdt(gdt, sizeof(gdt));
    load_segment_registers();

    const char msg[] = "[KERNEL] GDT Initialized... Success\n";
    flanterm_write(ft_ctx, msg, sizeof(msg));
}