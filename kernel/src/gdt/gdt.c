#include "gdt.h"

void encode_gdt_entry(uint8_t* target, struct gdt_structured source) {
    target[5] = source.access_byte;
    target[6] |= (source.flags << 4);
}

void gdt_structured_to_real(struct gdt* gdt_real, struct gdt_structured* gdt_structured, size_t total) {
    for (size_t i = 0; i < total; i++) {
        encode_gdt_entry((uint8_t*)&gdt_real[i], gdt_structured[i]);
    }
}