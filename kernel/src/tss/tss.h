#ifndef TSS_H
#define TSS_H

#include <stdint.h>

#define TSS_OFFSET 0x28
#define TSS_SIZE 0x67

struct tss {
    uint32_t reserved;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved_1_2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved_3_4;
    uint16_t reserved_5;
    uint16_t iopb;
}__attribute__((packed));

struct tss_descriptor {
    uint16_t segment_limit_0_15;
    uint16_t base_addr_0_15;
    uint8_t base_addr_16_23;
    uint16_t flags;
    uint8_t base_addr_24_31;
    uint32_t base_addr_32_63;
    uint32_t reserved;
}__attribute__((packed));

void init_tss();

#endif