#include "gdt/gdt.h"
#include "devices/timer.h"
#include "devices/hpet.h"
#include "mm/vmm.h"
#include "mm/pmm.h"
#include "idt/idt.h"
#include "drivers/terminal.h"
#include "mm/heap.h"
#include "firmware/acpi.h"
#include "klibc/io.h"
#include "klibc/string.h"
#include "klibc/alloc.h"
#include "devices/apic.h"
#include "devices/ioapic.h"
#include "devices/ps2.h"

// linker script accordingly.
void _start(void) {

    terminal_init();
    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();
    heap_init();
    acpi_init();
    apic_init();
    ioapic_init();
    ps2_init();
    hpet_init();
    apic_timer_init();

    // We're done, just hang...
    while (1) {}
}
