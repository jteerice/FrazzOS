#include "framebuffer.h"
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
    for (uint64_t i = 0; i < gtx_buffer.framebuffer_height; i++) {
        for (uint64_t j = 0; j < gtx_buffer.framebuffer_width; j++) {
            draw_pixel(i, j, ssfn_dst.bg);
        }
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
    for (int i = 0; i < framebuffer->mode_count; i++){ 
        if (framebuffer->modes[i]->width > gtx_buffer.framebuffer_width) {
            gtx_buffer.framebuffer_width = framebuffer->modes[i]->width;
            gtx_buffer.framebuffer_height = framebuffer->modes[i]->height;
            gtx_buffer.pitch = framebuffer->modes[i]->pitch;
            gtx_buffer.bpp = framebuffer->modes[i]->bpp;
        }
    }

    ssfn_src = (ssfn_font_t*)&_binary_src_fonts_lanapixel_sfn_start;
    ssfn_dst.ptr = gtx_buffer.framebuffer_addr;
    ssfn_dst.w = gtx_buffer.framebuffer_width;
    ssfn_dst.h = gtx_buffer.framebuffer_height;
    ssfn_dst.p = gtx_buffer.pitch;
    ssfn_dst.x = 0;
    ssfn_dst.y = 0;

    term_set_bg(DEFAULT_BG);
}