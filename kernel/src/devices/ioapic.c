#include "ioapic.h"
#include "mm/mm.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "firmware/acpi.h"
#include "klibc/alloc.h"
#include "status.h"

extern struct io_apic_t** io_apic_tbl;
extern struct ioso_apic_t** ioso_apic_tbl;
extern uint64_t ioso_apic_idx;
volatile uint32_t* ioapic_base;
volatile uint32_t gsi_base;
volatile uint8_t max_redirection_entry;

volatile uint32_t* io_reg_sel = (uint32_t*)(IO_REG_SEL_ADDR + KERNEL_VIRT_BASE_ADDR);
volatile uint32_t* io_win = (uint32_t*)(IO_WIN_ADDR + KERNEL_VIRT_BASE_ADDR);
volatile uint32_t* io_red_tbl = (uint32_t*)(IOREDTBL_BASE + KERNEL_VIRT_BASE_ADDR);

int irq_map[NUM_OF_IRQ] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint32_t read_ioapic_reg(uint32_t reg) {
    *(volatile uint32_t*)io_reg_sel = (volatile uint32_t)reg;
    return *(volatile uint32_t*)io_win;
}

void write_ioapic_reg(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)io_reg_sel = (volatile uint32_t)reg;
    *(volatile uint32_t*)io_win = (volatile uint32_t)data;
}

uint64_t read_redtbl_entry(uint8_t entry) {
    uint8_t offset = calc_redtbl_offset(entry);
    uint64_t res = read_ioapic_reg(offset);
    res |= (read_ioapic_reg(offset + 1) << 31);
    return res;
}

int mask_gsi(uint8_t idx) {
    uint64_t data = read_redtbl_entry(idx);
    if (idx > max_redirection_entry) {
        return EINVAL;
    }
    uint8_t offset = calc_redtbl_offset(idx);
    write_ioapic_reg(offset, data | (1 << 16));
    return 0;
}

void map_redtbl_entry(uint8_t entry, uint8_t vector) {
    union redirection_entry_t data;

    data.raw_data = read_redtbl_entry(entry);
    data.entry.vector = vector + IDT_EXCEPTIONS;
    data.entry.mask = 0;
    data.entry.destination = 0;

    uint8_t offset = calc_redtbl_offset(entry);
    write_ioapic_reg(offset, data.registers.lower);
    write_ioapic_reg(offset + 1, data.registers.upper);
}

void ioapic_init() {
    kprint("[KERNEL] IOAPIC Initializing... ");
    ioapic_base = (uint32_t*)((uintptr_t)io_apic_tbl[0]->io_apic_addr);
    gsi_base = io_apic_tbl[0]->gsib;

    max_redirection_entry = ((read_ioapic_reg(IOAPICVER) >> 16) & 0xff) & ~(1 << 7);

    for (uint8_t i = 0; i < max_redirection_entry + 1; i++) {
        mask_gsi(i);
    }

    for (uint64_t i = 0; i < ioso_apic_idx; i++) {
        map_redtbl_entry(ioso_apic_tbl[i]->gsi, ioso_apic_tbl[i]->irq);
        if (ioso_apic_tbl[i]->gsi < 16) {
            irq_map[ioso_apic_tbl[i]->gsi] = 1;
        }
    }

    for (uint8_t i = 0; i < NUM_OF_IRQ; i++) {
        if (!irq_map[i]) {
            map_redtbl_entry(i, i);
        }
    }

    // Mask all except keyboard interrupt
    for (uint8_t i = 0; i < NUM_OF_IRQ; i++) {
        if (i != 1) {
            mask_gsi(i);
        }
    }

    kprint("Success\n");
}