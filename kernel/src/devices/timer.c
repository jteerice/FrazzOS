#include "timer.h"
#include "ioapic.h"
#include "klibc/io.h"
#include "klibc/string.h"
#include "hpet.h"

#define APIC_TIMER_INTERVAL 10000

extern volatile uint32_t* lapic_base;
extern volatile struct hpet_table_t* hpet_table;
extern volatile uint64_t ticks_per_microsecond;
volatile uint32_t* lvt_timer_reg; 
volatile uint32_t* init_count_reg;
volatile uint32_t* current_count_reg;
volatile uint32_t* divide_config_reg;

void reset_apic_timer() {
    *init_count_reg = (volatile uint32_t)MAX_INIT_COUNT_VAL;
}

void apic_timer_init() {
    kprint("[KERNEL] APIC Timer Initializing... ");

    lvt_timer_reg = (uint32_t*)((uint64_t)lapic_base + LVT_TIMER_REG_OFFSET);
    init_count_reg = (uint32_t*)((uint64_t)lapic_base + INIT_COUNT_REG_OFFSET);
    current_count_reg = (uint32_t*)((uint64_t)lapic_base + CURRENT_COUNT_REG_OFFSET);
    divide_config_reg = (uint32_t*)((uint64_t)lapic_base + DIVIDE_CONFIG_REG_OFFSET);

    reset_hpet_timer();
    reset_apic_timer();
    start_hpet_timer();
    while (poll_time_hpet_timer() < ((uint64_t)(FEMTOSECONDS_PER_MICROSECOND) * APIC_TIMER_INTERVAL)) {}
    mask_irq(TIMER_IRQ);
    uint64_t apic_ticks_10ms = (uint64_t)MAX_INIT_COUNT_VAL - (uint64_t)*current_count_reg;
    *lvt_timer_reg |= ((TIMER_IRQ + IDT_EXCEPTIONS) | TIMER_PERIODIC_MODE);
    *divide_config_reg = 0x03;
    *init_count_reg = (uint32_t)apic_ticks_10ms;
    unmask_irq(TIMER_IRQ);
    
    kprint("Success\n");
}