#ifndef SMP_H
#define SMP_H

#include <stdint.h>
#include "tss/tss.h"

#define GS_BASE 0xC00001011

struct cpu_local {
    uint64_t meta_ptr;
    uint64_t thread_kernel_stack;
    uint64_t thread_user_stack;
    uint64_t in_irq;
    struct tss tss;
}__attribute__((packed));

void smp_init();
void new_cpu_local();
struct cpu_local* get_cpu_local();
void write_msr(uint64_t msr, uint64_t val);

#endif