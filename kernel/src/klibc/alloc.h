#ifndef ALLOC_H
#define ALLOC_H

#include <stdint.h>

void* malloc(uint64_t size);
void* mmap(uintptr_t addr, uint64_t size, int flags);

#endif