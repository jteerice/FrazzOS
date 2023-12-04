#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define LVT_TIMER_REG_OFFSET 0x320
#define INIT_COUNT_REG_OFFSET 0x380
#define CURRENT_COUNT_REG_OFFSET 0x390
#define DIVIDE_CONFIG_REG_OFFSET 0x3e0

#define MAX_INIT_COUNT_VAL 0xffffffff
#define TIMER_PERIODIC_MODE 0x20000

void apic_timer_init();

#endif