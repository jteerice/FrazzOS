#ifndef TSS_H
#define TSS_H

#include <stdint.h>

struct tss {
    uint16_t iopb;
    uint16_t reserved_1;
    uint32_t reserved_2;
    uint32_t reserved_3;
    uint32_t ist7_high;
    uint32_t ist7_low;
    uint32_t ist6_high;
    uint32_t ist6_low;
    uint32_t ist5_high;
    uint32_t ist5_low;
    uint32_t ist4_high;
    uint32_t ist4_low;
    uint32_t ist3_high;
    uint32_t ist3_low;
    uint32_t ist2_high;
    uint32_t ist2_low;
    uint32_t ist1_high;
    uint32_t ist1_low;
    uint32_t reserved_4;
    uint32_t reserved_5;
    uint32_t rsp2_high;
    uint32_t rsp2_low;
    uint32_t rsp1_high;
    uint32_t rsp1_low;
    uint32_t rsp0_high;
    uint32_t rsp0_low;
    uint32_t reserved_6;
};

void init_tss();
void load_tss(struct tss* tss);

#endif