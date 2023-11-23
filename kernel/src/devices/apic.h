#ifndef APIC_H
#define APIC_H

#include <stdint.h>

#define APIC_SPURIOUS_VECTOR_REG 0xf
#define APIC_EOI_REG 0xb0
#define APIC_ENABLE (1 << 8)
#define SPURIOUS_INT 0xff

void apic_init();
void lapic_write_reg(uint32_t reg, uint32_t data);

#endif