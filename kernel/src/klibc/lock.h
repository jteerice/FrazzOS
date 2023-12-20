#ifndef LOCK_H
#define LOCK_H

#include <stdatomic.h>

void lock(atomic_flag* lock);
void unlock(atomic_flag* lock);

#endif