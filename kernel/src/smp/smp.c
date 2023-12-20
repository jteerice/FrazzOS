#include "smp.h"
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