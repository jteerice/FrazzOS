#include "string.h"
#include "memory.h"
#include "io.h"

int strlen(const char* str) {
    int res = 0;
    while (*str++) res++;
    return res;
}

char* ull_to_hex(char* buf, uint64_t num) {
    static char chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    memset(buf, 48, HEX_STRING_MAX);
    buf[0] = '0';
    buf[1] = 'x';
    buf[HEX_STRING_MAX] = 0;

    char* str = &buf[HEX_STRING_MAX - 1];

    while (num) {
        *--str = chars[num % HEX_BASE];
        num /= HEX_BASE;
    }

    return buf;
}

