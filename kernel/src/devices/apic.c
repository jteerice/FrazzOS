#include "apic.h"
#include "cpu.h"
#include "klibc/io.h"
#include "firmware/acpi.h"
#include "mm/mm.h"
#include "pic.h"
#include <stdbool.h>

extern void cpuid_proc_feat_info(struct cpuid_registers_t* regs);
extern struct madt_t* madt;
uintptr_t lapic_base;

void lapic_write_reg(uint32_t reg, uint32_t data) {
    *((volatile uint32_t*)(lapic_base + reg)) = data;
}

static void enable_lapic() {
    lapic_write_reg(APIC_SPURIOUS_VECTOR_REG, APIC_ENABLE | SPURIOUS_INT);
}

static bool is_apic_available() {
    struct cpuid_registers_t regs;
    cpuid_proc_feat_info(&regs);
    return regs.edx & CPUID_FEAT_EDX_APIC;
}

void apic_init() {
    kprint("[KERNEL] APIC Initializing... ");
    if (!is_apic_available()) {
        kprint("[[[PANIC]]] No APIC Found!");
        for (;;) {
            asm ("hlt");
        }
    }

    lapic_base = phys_to_hh((uintptr_t)madt->lapic_addr);
    remap_pic();
    disable_pic();
    enable_lapic();
    kprint("Success\n");
}
