#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stdbool.h>

#define BYTES_PER_LONG 8

int find_first_free(uint64_t start, uint64_t* buffer, uint64_t buffer_size);
int is_free_region_big_enough(int start, uint64_t* buffer, uint64_t num_bytes);
bool bitmap_test_bit(uint64_t idx, uint64_t* buffer);
void bitmap_set_bit(uint64_t idx, uint64_t* buffer);

#endif