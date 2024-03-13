#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

#define HEX_STRING_MAX 18
#define HEX_BASE 16
#define MAX_NUM_STRING_LEN 50
#define BASE_10 10

int strlen(const char* str);
char* ull_to_hex(char* buf, uint64_t num);
char* itoa(char* buf, uint64_t num);
int strncpy(char* dst, const char* src, size_t size);

#endif