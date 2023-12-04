#include "hpet.h"
#include "mm/mm.h"
#include "mm/vmm.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "firmware/acpi.h"

volatile struct hpet_t* hpet;
volatile struct hpet_table_t* hpet_table;
volatile uint32_t minimum_tick;

void hpet_init() {
    kprint("[KERNEL] HPET Intializing... ");

    hpet = (struct hpet_t*)acpi_find_sdt_tbl("HPET");
    if (!hpet) {
        kprint("[[[PANIC]]] NO HPET FOUND\n");
        for (;;) {
            asm ("hlt");
        }
    } 

    hpet_table = (volatile struct hpet_table_t*)phys_to_hh(hpet->address_info.address);
    minimum_tick = hpet_table->capabilities >> 32;
    uint64_t frequency = FREQUENCY_DIVISOR / minimum_tick;
    hpet_table->config |= ENABLE_LEGACY_REPLACE;
    if (!(hpet_table->config & ENABLE_HPET)) {
        hpet_table->main_counter = 0;
    }
    hpet_table->timers[0].cap_config |= (TIMER_ENABLE_INTERRUPT | ENABLE_PERIODIC_MODE | ENABLE_SET_ACCUMULATOR);
    hpet_table->timers[0].comparator = frequency / FREQUENCY_HERTZ;
    hpet_table->config |= ENABLE_HPET;

    kprint("Success\n");
}