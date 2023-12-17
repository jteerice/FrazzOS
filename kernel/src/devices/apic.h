#ifndef APIC_H
#define APIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#define APIC_SPURIOUS_VECTOR_REG 0xf0
#define APIC_EOI_REG 0xb0
#define APIC_ENABLE (1 << 8)
#define SPURIOUS_INT 0xff

enum LAPIC_REGS {
    LAPIC_ID_REG = 0x20,
    LAPIC_VER_REG = 0x30,
    TASK_PRIO_REG = 0x80,
    ARBITRATION_PRIO_REG = 0x90,
    PROC_PRIO_REG = 0xa0,
    EOI_REG = 0xb0,
    REMOTE_READ_REG = 0xc0,
    LOGICAL_DEST_REG = 0xd0,
    DEST_FORMAT_REG = 0xe0,
    SPURIOUS_INT_VECT_REG = 0xf0,
    IN_SERVICE_REG = 0x100,
    TRIGGER_MODE_REG = 0x180,
    INTERRUPT_REQ_REG = 0x200,
    ERROR_STAT_REG = 0x280,
    LVT_CMCI_REG = 0x2f0,
    INTERRUPT_COMMAND_REG_1 = 0x300,
    INTERRUPT_COMMAND_REG_2 = 0x310,
    LVT_TIMER_REG = 0x320,
    LVT_THERMAL_REG = 0x330,
    LVT_PERFORMANCE_REG = 0x340,
    LVT_LINT0_REG = 0x350,
    LVT_LINT1_REG = 0x360,
    LVT_ERROR_REG = 0x370,
    INIT_COUNT_REG = 0x380,
    CURRENT_COUNT_REG = 0x390,
    DIVIDE_CONFIG_REG = 0x3e0
};

void apic_init();
void lapic_write_reg(uint32_t reg, uint32_t data);
uint32_t lapic_read_reg(uint32_t reg);

#endif