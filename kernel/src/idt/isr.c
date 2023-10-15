#include "isr.h"
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;

void general_exception_handler() {
    const char msg[] = "No exception handler implemented!\n";
    flanterm_write(ft_ctx, msg, sizeof(msg));
    asm volatile("cli; hlt");
}

void divide_by_zero_exception_handler() {
    const char msg[] = "Divide by zero exception!\n";
    flanterm_write(ft_ctx, msg, sizeof(msg));
    asm volatile("cli; hlt");
}