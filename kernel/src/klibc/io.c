#include "io.h"
#include "drivers/graphics/framebuffer.h"
#include "string.h"
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;

void kprint(char* str) {
    term_print_string(str, PURPLE);
    /*int size = strlen(str);
    flanterm_write(ft_ctx, str, size);*/
}
