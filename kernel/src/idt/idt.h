#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_GATE_INTERRUPT 0b10001110
#define IDT_GATE_TRAP      0b10001111

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
}__attribute__((packed));

struct idt_reg {
    uint16_t limit;
    uint32_t base;
}__attribute((packed));

void idt_set_gate(uint8_t interrupt_code, void* interrupt_routine, uint8_t flags);
void idt_init();

#endif