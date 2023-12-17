#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#define DEFAULT_BG 0x0B060F
#define PURPLE 0x734AB9

struct graphic_buffer {
    void* framebuffer_addr;
    uint64_t framebuffer_width;
    uint64_t framebuffer_height;
    uint64_t pitch;
    uint16_t bpp;
    int glyph_width;
    int glyph_height;
};

void framebuffer_init();
void term_print_string(char* str, uint32_t color);
void term_line_shift_up();

#endif