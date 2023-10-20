#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#define HEX_STRING_MAX 19
#define HEX_BASE 16

int strlen(const char* str);
char* ull_to_hex(char* buf, uint64_t num);

#endif