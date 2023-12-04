#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

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

#endif