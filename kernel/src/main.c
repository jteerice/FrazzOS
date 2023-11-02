#include "gdt/gdt.h"
#include "mm/vmm.h"
#include "mm/pmm.h"
#include "idt/idt.h"
#include "drivers/terminal.h"
#include "mm/heap.h"

// linker script accordingly.
void _start(void) {

    terminal_init();
    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();
    heap_init();

    // We're done, just hang...
    while (1) {}
}
