#include "terminal.h"
#include <flanterm.h>
#include <backends/fb.h>


static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};
struct flanterm_context* ft_ctx;

static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

void terminal_init() {
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }
    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    ft_ctx = flanterm_fb_simple_init(
        framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch
    );

}