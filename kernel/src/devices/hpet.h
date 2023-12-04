#ifndef HPET_H
#define HPET_H

#include <stdint.h>

#define CONFIG_REGISTER_OFFSET 0x10
#define INTERRUPT_STATUS_REGISTER_OFFSET 0x20
#define MAIN_COUNTER_REGISTER_OFFSET 0xf0

#define MICROSECOND_DIVISOR 1000000000
#define MICROSECOND_SCALAR 0x100000000000000
#define ENABLE_HPET 0x01

/*struct general_capabilities_reg {
    uint8_t rev_id;
    uint8_t num_timers : 5;
    uint8_t count_size_cap : 1;
    uint8_t reserved : 1;
    uint8_t leg_replacement_cap : 1;
    uint16_t vendor_id;
    uint32_t counter_clock_period;
}__attribute__((packed));

struct general_config_reg {
    uint8_t enable : 1;
    uint8_t leg_replacement_enable : 1;
    uint32_t reserved_1 : 30;
    uint32_t reserved_2;
}__attribute__((packed));

struct interrupt_status_reg {
    uint32_t comparator_trigger_reg;
    uint32_t reserved;
}__attribute__((packed));

struct counter_value_reg {
    volatile uint64_t counter_val;
}__attribute__((packed));

struct timer_config_capability_reg {
    uint8_t reserved_1 : 1;
    uint8_t interrupt_type : 1;
    uint8_t interrupt_enable : 1;
    uint8_t type : 1;
    uint8_t period_int_cap : 1;
    uint8_t size_cap : 1;
    uint8_t value_set : 1;
    uint8_t reserved_2 : 1;
    uint8_t mode_32 : 1;
    uint8_t ioapic_route : 5;
    uint8_t fsb_enable : 1;
    uint8_t fsb_cap : 1;
    uint16_t reserved_3;
    uint32_t irq_route; 
}__attribute__((packed));*/

struct hpet_registers_t {
    volatile uint64_t* capabilities;
    volatile uint64_t* config;
    volatile uint64_t* int_status;
    volatile uint64_t* counter_value;
};

void hpet_init();

#endif