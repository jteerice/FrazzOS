#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <stddef.h>

struct gdt {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
}__attribute__((packed));

struct gdt_structured {
    uint8_t access_byte;
    uint8_t flags;
};

void load_gdt(struct gdt* gdt, size_t size);
void gdt_structured_to_real(struct gdt* gdt, struct gdt_structured* gdt_structured, size_t total);
void load_segment_registers();

#endif