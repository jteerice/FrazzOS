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

void framebuffer_init() {
    struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];
    gtx_buffer.framebuffer_addr = framebuffer->address;
    gtx_buffer.framebuffer_width = framebuffer->width;
    gtx_buffer.framebuffer_height = framebuffer->height;
    gtx_buffer.pitch = framebuffer->pitch;
    gtx_buffer.bpp = framebuffer->bpp;
    gtx_buffer.glyph_height = 16;
    gtx_buffer.glyph_width = 8;

    ssfn_src = &_binary_src_fonts_lanapixel_sfn_start;
    ssfn_dst.ptr = gtx_buffer.framebuffer_addr;
}