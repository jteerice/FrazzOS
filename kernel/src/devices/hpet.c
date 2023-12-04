#include "hpet.h"
#include "mm/mm.h"
#include "mm/vmm.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "firmware/acpi.h"

extern uint64_t* root_page_dir;
struct hpet_t* hpet;
volatile uint64_t* hpet_base;
volatile uint64_t* capabilities_reg;
volatile uint64_t* config_reg;
volatile uint64_t* counter_reg;
volatile uint64_t* interrupt_status_reg;

void hpet_init() {
    kprint("[KERNEL] HPET Intializing... ");

    hpet = (struct hpet_t*)acpi_find_sdt_tbl("HPET");
    if (!hpet) {
        kprint("[[[PANIC]]] NO HPET FOUND\n");
        for (;;) {
            asm ("hlt");
        }
    } 
    hpet_base = (volatile uint64_t*)phys_to_hh(hpet->address_info.address);
    capabilities_reg = (volatile uint64_t*)hpet_base;
    config_reg = (volatile uint64_t*)((uint64_t)hpet_base + CONFIG_REGISTER_OFFSET);
    counter_reg = (volatile uint64_t*)((uint64_t)hpet_base + MAIN_COUNTER_REGISTER_OFFSET);
    interrupt_status_reg = (volatile uint64_t*)((uint64_t)hpet_base + INTERRUPT_STATUS_REGISTER_OFFSET);
    uint32_t minimum_tick = *(uint64_t*)(capabilities_reg) >> 32;
    uint64_t microsecond = MICROSECOND_DIVISOR / minimum_tick;
    if (!(*config_reg & ENABLE_HPET)) {
        *counter_reg = (volatile uint64_t)(MICROSECOND_SCALAR - microsecond);
    }
    *config_reg |= ENABLE_HPET;

    kprint("Success\n");
}