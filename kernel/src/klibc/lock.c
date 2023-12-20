#include "lock.h"

void lock(atomic_flag* lock) {
    while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
        asm volatile ("pause": : : "memory");
    }
}

void unlock(atomic_flag* lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}