#include "gdt/gdt.h"
#include "idt/idt.h"
#include "drivers/terminal.h"

// linker script accordingly.
void _start(void) {

    terminal_init();
    gdt_init();
    idt_init();

    // We're done, just hang...
    while (1) {}
}
