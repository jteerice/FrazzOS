#include "smp.h"
#include "klibc/lock.h"
#include "klibc/io.h"
#include <limine.h>

struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

atomic_flag smp_lock = ATOMIC_FLAG_INIT;

void ap_entry_point () {
    kprint("Hello from APs!\n");
    while (1) {}
}

void smp_init() {
    atomic_flag* mutex_lock = &smp_lock;
    struct limine_smp_response* smp_response = smp_request.response;
    for (int i = 1; i < (int)smp_response->cpu_count; i++) {
        lock(mutex_lock);
        smp_response->cpus[i]->goto_address = ap_entry_point;
        unlock(mutex_lock);
    }
}