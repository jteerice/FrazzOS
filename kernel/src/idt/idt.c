#include "idt.h"
#include "isr.h"
#include "config.h"
#include <stddef.h>
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;
extern void* isr_stub_table[FRAZZOS_TOTAL_IDT_ENTRIES];
extern void load_idt(struct idt_entry* idt_entry, int size);

struct idt_entry idt[FRAZZOS_TOTAL_IDT_ENTRIES];
struct idt_reg idt_reg;

void idt_set_entry(uint8_t interrupt_code, void* interrupt_routine, uint8_t flags) {
    struct idt_entry* descriptor = &idt[interrupt_code];

    descriptor->isr_low = (uint64_t)interrupt_routine & 0xFFFF;
    descriptor->kernel_cs = KERNEL_CS;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)interrupt_routine & 0xFFFF0000) >> 16;
    descriptor->isr_high = ((uint64_t)interrupt_routine & 0xFFFFFFFF00000000) >> 32;
    descriptor->reserved = 0;
}

void idt_init() {
    idt_reg.limit = sizeof(idt) - 1;
    idt_reg.base = (uint64_t)&idt;

    for (int i = 0; i < FRAZZOS_TOTAL_IDT_ENTRIES; i++) {
        idt_set_entry((uint8_t)i, isr_stub_table[i], IDT_GATE_INTERRUPT);
    }

    load_idt(idt, (sizeof(idt) - 1));

    const char msg[] = "[KERNEL] IDT Initialized... Success\n";
    flanterm_write(ft_ctx, msg, sizeof(msg));
}