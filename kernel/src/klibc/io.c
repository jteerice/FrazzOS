#include "io.h"
#include "drivers/graphics/framebuffer.h"
#include "string.h"
#include "lock.h"
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;

atomic_flag spinlock = ATOMIC_FLAG_INIT;

void kprint(char* str) {
    lock(&spinlock);
    term_print_string(str, PURPLE);
    unlock(&spinlock);
    /*int size = strlen(str);
    flanterm_write(ft_ctx, str, size);*/
}
