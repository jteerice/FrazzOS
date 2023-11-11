#include "gdt/gdt.h"
#include "mm/vmm.h"
#include "mm/pmm.h"
#include "idt/idt.h"
#include "drivers/terminal.h"
#include "mm/heap.h"
#include "firmware/acpi.h"
#include "klibc/io.h"
#include "klibc/string.h"
#include "klibc/alloc.h"

// linker script accordingly.
void _start(void) {

    terminal_init();
    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();
    heap_init();
    //acpi_init();

    char buf[HEX_STRING_MAX];
    void* ptr1 = malloc(256);
    kprint("ptr1 @ ");
    kprint(ull_to_hex(buf, (uint64_t)ptr1));
    kprint("\n");
    void* ptr2 = malloc(256);
    kprint("ptr2 @ ");
    kprint(ull_to_hex(buf, (uint64_t)ptr2));
    kprint("\n");

    
    // We're done, just hang...
    while (1) {}
}
