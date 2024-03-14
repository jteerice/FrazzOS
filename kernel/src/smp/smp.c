#include "smp.h"
#include "klibc/alloc.h"
#include "klibc/string.h"
#include "klibc/lock.h"
#include "klibc/io.h"
#include <limine.h>

struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

atomic_flag ap_done_lock = ATOMIC_FLAG_INIT;
volatile int ap_done;

void ap_entry_point () {
    lock(&ap_done_lock);
    ap_done--;
    unlock(&ap_done_lock);
    while (1) {}
}

void write_msr(uint64_t msr, uint64_t val) {
    uint64_t a = val & 0xFFFFFFFF;
    uint64_t d = val >> 32;
    asm volatile("wrmsr" :: "a" (a), "d" (d), "c" (msr));
}

struct cpu_local* get_cpu_local() {
    struct cpu_local* res;
    asm volatile("movq %%gs:(0), %0;" : "=r" (res));
    return res;
}

void new_cpu_local() {
    struct cpu_local* new = malloc(sizeof(struct cpu_local));
    new->meta_ptr = (uint64_t)new;
    write_msr(GS_BASE, (uint64_t)new);
}    

void smp_init() {
    kprint("[KERNEL] Initializing APs...\n");
    struct limine_smp_response* smp_response = smp_request.response;
    ap_done = smp_response->cpu_count - 1;
    for (int i = 1; i < (int)smp_response->cpu_count; i++) {
        kprint("\t [AP] AP Booted\n");
        smp_response->cpus[i]->goto_address = ap_entry_point;
    }
    while (ap_done > 0) {}
    kprint("[KERNEL] Success\n");
}