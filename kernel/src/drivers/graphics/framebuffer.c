#include "framebuffer.h"
#include "klibc/alloc.h"
#include <limine.h>

#include <stddef.h>

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_NOIMPLEMENTATION
#include <ssfn.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

extern uint8_t _binary_src_fonts_lanapixel_sfn_start;
struct graphic_buffer gtx_buffer;

void draw_pixel(int x, int y, uint32_t color) {
    uint64_t idx = y * (gtx_buffer.pitch / sizeof(int)) + x;
    uint32_t* framebuffer = (uint32_t*)gtx_buffer.framebuffer_addr;
    framebuffer[idx] = color;
}

void term_set_bg(uint32_t bg_color) {
    ssfn_dst.bg = bg_color;
    for (uint64_t i = 0; i < gtx_buffer.framebuffer_width; i++) {
        for (uint64_t j = 0; j < gtx_buffer.framebuffer_height; j++) {
            draw_pixel(i, j, ssfn_dst.bg);
        }
    }
}

void term_line_shift_up() {
    uint32_t* framebuffer = (uint32_t*)gtx_buffer.framebuffer_addr;
    for (uint64_t i = gtx_buffer.glyph_height; i < gtx_buffer.framebuffer_width; i++) {
        for (uint64_t j = 0; j < gtx_buffer.framebuffer_height; j++) {
            uint64_t idx = i * (gtx_buffer.pitch / sizeof(int)) + j;
            uint64_t current_color = framebuffer[idx];
            framebuffer[idx] = ssfn_dst.bg;
            idx = (i - gtx_buffer.glyph_height) * (gtx_buffer.pitch / sizeof(int)) + j;
            framebuffer[idx] = current_color;
        }
    }
}

void term_print_char(uint32_t unicode, int x, int y, uint32_t color) {
    ssfn_dst.x = x;
    ssfn_dst.y = y;
    ssfn_dst.fg = color;

    if ((uint64_t)(ssfn_dst.x + gtx_buffer.glyph_width) > gtx_buffer.framebuffer_width) {
        ssfn_dst.x = 0;
        ssfn_dst.y += gtx_buffer.glyph_height;
    }
    if ((uint64_t)ssfn_dst.y >= gtx_buffer.framebuffer_height) {
        ssfn_dst.x = 0;
        ssfn_dst.y = gtx_buffer.framebuffer_height - gtx_buffer.glyph_height;
        term_line_shift_up();
    }
    if (unicode == '\n') {
        ssfn_dst.x = 0;
        ssfn_dst.y += gtx_buffer.glyph_height;
        return;
    } else if (unicode == '\t') {
        for (uint8_t i = 0; i < 4; i++) {
            term_print_char(' ', ssfn_dst.x, ssfn_dst.y, color);
        }
        return;
    } else if (unicode == '\b') {
        if (ssfn_dst.x == 0) {
            if (ssfn_dst.y == 0) {
                return;
            }
            ssfn_dst.x = gtx_buffer.framebuffer_width - gtx_buffer.glyph_width;
            ssfn_dst.y -= gtx_buffer.glyph_height;
            ssfn_putc(' ');
            ssfn_dst.x -= gtx_buffer.glyph_width;
        } else {
            ssfn_dst.x -= gtx_buffer.glyph_width;
            ssfn_putc(' ');
            ssfn_dst.x -= gtx_buffer.glyph_width;
        }
        return;
    } 

    ssfn_putc(unicode);
}

void term_print_string(char* str, uint32_t color) {
    while (*str) {
        term_print_char(ssfn_utf8(&str), ssfn_dst.x, ssfn_dst.y, color);
    }
}

void framebuffer_init() {
    struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];
    gtx_buffer.framebuffer_addr = framebuffer->address;
    gtx_buffer.framebuffer_width = framebuffer->width;
    gtx_buffer.framebuffer_height = framebuffer->height;
    gtx_buffer.pitch = framebuffer->pitch;
    gtx_buffer.bpp = framebuffer->bpp;
    gtx_buffer.glyph_height = 16;
    gtx_buffer.glyph_width = 8;

    ssfn_src = (ssfn_font_t*)&_binary_src_fonts_lanapixel_sfn_start;
    ssfn_dst.ptr = gtx_buffer.framebuffer_addr;
    ssfn_dst.w = gtx_buffer.framebuffer_width;
    ssfn_dst.h = gtx_buffer.framebuffer_height;
    ssfn_dst.p = gtx_buffer.pitch;
    ssfn_dst.x = 0;
    ssfn_dst.y = 0;

    term_set_bg(DEFAULT_BG);
    term_print_string("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n", PURPLE);
    term_print_string("\n", PURPLE);
    term_print_string("Kernel Booted -- Welcome to FrazzOS 64-bit!\n", PURPLE);
    term_print_string("\n", PURPLE);
    term_print_string("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n", PURPLE);
}