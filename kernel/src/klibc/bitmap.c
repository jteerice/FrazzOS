#include "bitmap.h"
#include "mm/mm.h"

int find_first_free(uint64_t start, uint64_t* buffer, uint64_t buffer_size) {
    int start_block = start / BITS_PER_BLOCK;
    if (start % BITS_PER_BLOCK != 0) {
        int start_bit = start % BITS_PER_BLOCK;
        for (int i = start_bit; i < BITS_PER_BLOCK; i++) {
            if (!(buffer[start_block] & (1 << i))) {
                return start_block * BITS_PER_BLOCK + i; 
            }
        } 
        start_block++;
    }
    for (uint64_t i = start_block; i < buffer_size / BITS_PER_BLOCK; i++) {
        if (buffer[i] != 0xFFFFFFFFFFFFFFFF) {
            for (int j = 0; j < BITS_PER_BLOCK; j++) {
                if (!(buffer[i] & (1 << j))) {
                    return i * BITS_PER_BLOCK + j;
                }
            }
        }
    }
    return ENOMEM;