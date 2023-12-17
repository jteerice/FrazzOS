#include "smp.h"
#include "klibc/alloc.h"
#include "mm/pmm.h"
#include "mm/mm.h"
#include "klibc/memory.h"
#include "gdt/gdt.h"
#include "klibc/io.h"
#include "klibc/string.h"
#include "devices/apic.h"
#include "firmware/acpi.h"
#include "devices/hpet.h"
#include "config.h"

extern uintptr_t lapic_base;
extern struct proc_lapic_t** proc_apic_tbl;
extern uint64_t proc_apic_idx;
extern uint64_t lapic_id;
extern struct gdt gdt[FRAZZOS_TOTAL_SEGMENTS];
extern uint64_t* root_page_dir;

extern char smp_check_flag();
extern void prepare_ap_trampoline(uint64_t* pm14, volatile uint8_t* stack);
extern unsigned char ap_trampoline[];

volatile struct ap_stack* cpu_stacks;

void send_init_ipi(uint8_t apic_id) {
        lapic_write_reg(ERROR_STAT_REG, 0);
        lapic_write_reg(INTERRUPT_COMMAND_REG_2, (apic_id << 24));
        lapic_write_reg(INTERRUPT_COMMAND_REG_1, INIT_IPI_MSG);

        do {
            asm volatile (
                "pause": : :
                "memory"
            );
        } while ((volatile uint32_t)lapic_read_reg(INTERRUPT_COMMAND_REG_1) & (1 << 12));

        lapic_write_reg(INTERRUPT_COMMAND_REG_2, (apic_id << 24));
        lapic_write_reg(INTERRUPT_COMMAND_REG_1, INIT_IPI_DEASSERT);

        do {
            asm volatile (
                "pause": : :
                "memory"
            );
        } while ((volatile uint32_t)lapic_read_reg(INTERRUPT_COMMAND_REG_1) & (1 << 12));

        wait_10ms();
}

int send_startup_ipi(uint8_t apic_id) {
    for (uint8_t i = 0; i < 2; i++) {
        prepare_ap_trampoline(root_page_dir, &cpu_stacks[apic_id - 1].stack[0]);
        lapic_write_reg(ERROR_STAT_REG, 0);
        lapic_write_reg(INTERRUPT_COMMAND_REG_2, (apic_id << 24));
        lapic_write_reg(INTERRUPT_COMMAND_REG_1, SIPI_CODE | (TRAMPOLINE_ADDR / PAGE_SIZE));

        do {
            asm volatile (
                "pause": : :
                "memory"
            );
        } while ((volatile uint32_t)lapic_read_reg(INTERRUPT_COMMAND_REG_1) & (1 << 12));

        if (i == 0) {
            for (int i = 0; i < 200; i++) {
                usleep(1);
                if (smp_check_flag()) {
                    return 0;
                }
            }
        } else {
            for (int i = 0; i < 10000; i++) {
                usleep(1);
                if (smp_check_flag()) {
                    return 0;
                }
            }
        }
    }
    return -1;
}

void ap_entry() {
    kprint("Hello from AP's!\n");
    while (1) {}
}

void smp_init() {
    kprint("[KERNEL] Initilizing APs... ");
    cpu_stacks = (volatile struct ap_stack*)phys_to_hh((uintptr_t)pmm_alloc((proc_apic_idx - 1) * sizeof(struct ap_stack)));
    char buf[HEX_STRING_MAX];
    kprint(ull_to_hex(buf, (uint64_t)((void*)TRAMPOLINE_ADDR)));
    kprint("\n");
    kprint(ull_to_hex(buf, (uint64_t)ap_trampoline));
    kprint("\n");
    memcpy((void*)(TRAMPOLINE_ADDR + KERNEL_VIRT_BASE_ADDR), ap_trampoline, PAGE_SIZE);
    for (uint8_t i = 0; i < proc_apic_idx; i++) {
        if (i == lapic_id) {
            continue;
        }
        send_init_ipi(i);
        send_startup_ipi(i);
    }
    kprint("Success\n");
}