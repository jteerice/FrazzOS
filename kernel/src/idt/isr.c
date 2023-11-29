#include "isr.h"
#include "klibc/io.h"
#include "devices/apic.h"
#include <flanterm.h>
#include <backends/fb.h>

extern struct flanterm_context* ft_ctx;

void general_exception_handler() {
    kprint("No exception handler implemented!\n");
    asm volatile("cli; hlt");
}

void divide_by_zero_exception_handler() {
    kprint("Divide by zero exception!\n");
    asm volatile("cli; hlt");
}

void keyboard_irq_handler() {
    kprint("Key pressed!\n");
    lapic_write_reg(APIC_EOI_REG, 0);
}

void timer_irq_handler() {
    kprint("Timer interrupt!\n");
    lapic_write_reg(APIC_EOI_REG, 0);
}