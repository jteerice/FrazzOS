#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <stddef.h>

struct gdt {
    uint16_t limit;     // ignored in 64 bit
    uint16_t base_low;  // ignored
    uint8_t base_mid;   // ignored
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;  // ignored
}__attribute__((packed));

struct gdt_structured {
    uint8_t access_byte;
    uint8_t flags;
};

void gdt_init();
void load_gdt(struct gdt* gdt, size_t size);
void gdt_structured_to_real(struct gdt* gdt, struct gdt_structured* gdt_structured, size_t total);
void load_segment_registers();

#endif