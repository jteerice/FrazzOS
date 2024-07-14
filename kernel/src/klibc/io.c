#include "io.h"
#include "drivers/graphics/framebuffer.h"
#include "string.h"
#include "../devices/ioapic.h"
#include "lock.h"
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;

atomic_flag spinlock = ATOMIC_FLAG_INIT;

void kprint(char* str) {
    asm volatile ("cli" ::: "memory");
    lock(&spinlock);
    term_print_string(str, PURPLE);
    unlock(&spinlock);
    asm volatile ("sti" ::: "memory");
}
