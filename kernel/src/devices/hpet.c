#include "hpet.h"
#include "ioapic.h"
#include "mm/mm.h"
#include "mm/vmm.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "firmware/acpi.h"

volatile struct hpet_t* hpet;
volatile struct hpet_table_t* hpet_table;
volatile uint32_t femto_per_tick;
volatile uint64_t ticks_per_microsecond;

void reset_hpet_timer() {
    hpet_table->main_counter = 0;
}

void start_hpet_timer() {
    hpet_table->config |= ENABLE_HPET;
}

uint64_t poll_ticks_hpet_timer() {
    return hpet_table->main_counter;
}

uint64_t poll_time_hpet_timer() {
    return hpet_table->main_counter * femto_per_tick;
}

void hpet_init() {
    kprint("[KERNEL] HPET Initializing... ");

    hpet = (struct hpet_t*)acpi_find_sdt_tbl("HPET");
    if (!hpet) {
        kprint("[[[PANIC]]] NO HPET FOUND\n");
        for (;;) {
            asm ("hlt");
        }
    } 

    hpet_table = (volatile struct hpet_table_t*)phys_to_hh(hpet->address_info.address);
    femto_per_tick = hpet_table->capabilities >> 32;
    ticks_per_microsecond = FEMTOSECONDS_PER_MICROSECOND / femto_per_tick;
    hpet_table->config &= ~(ENABLE_LEGACY_REPLACE | ENABLE_HPET);

    // The following code is useful for periodic timer that triggers interrupts on IRQ 0
    /*hpet_table->config |= ENABLE_LEGACY_REPLACE;
    if (!(hpet_table->config & ENABLE_HPET)) {
        hpet_table->main_counter = 0;
    }
    hpet_table->timers[0].cap_config |= (TIMER_ENABLE_INTERRUPT | ENABLE_PERIODIC_MODE | ENABLE_SET_ACCUMULATOR);
    hpet_table->timers[0].comparator = frequency / FREQUENCY_HERTZ;
    hpet_table->config |= ENABLE_HPET;
    unmask_irq(TIMER_IRQ);*/

    kprint("Success\n");
}