#include "tss.h"
#include "smp/smp.h"
#include "klibc/memory.h"
#include "klibc/alloc.h"
#include "klibc/io.h"

extern struct gdt gdt;

void init_tss() {
        kprint("[KERNEL] Initializing TSS... ");
        struct tss* tss_ptr = &get_cpu_local()->tss;
        struct tss_descriptor* tss_descriptor   = (struct tss_descriptor*)&gdt + TSS_OFFSET;
        tss_descriptor->base_addr_0_15          = (uint16_t)((uint64_t)tss_ptr & 0xFFFF);
        tss_descriptor->base_addr_16_23         = (uint8_t)((uint64_t)tss_ptr & 0xFF0000) >> 16;
        tss_descriptor->base_addr_24_31         = (uint8_t)((uint64_t)tss_ptr & 0xFF000000) >> 24;
        tss_descriptor->base_addr_32_63         = (uint32_t)((uint64_t)((uint64_t)tss_ptr & 0xFFFFFFFF00000000) >> 32);
        tss_descriptor->segment_limit_0_15      = (uint16_t)TSS_SIZE;
        tss_descriptor->flags                   = (uint16_t)(0b1001 | (1 << 7));
        asm volatile("ltr %%ax" :: "a"(TSS_OFFSET));
        kprint("Success\n");
}