#include "tss.h"
#include "klibc/memory.h"
#include "klibc/alloc.h"
#include "klibc/io.h"

struct tss* tss;

void init_tss() {
    tss = malloc(sizeof(struct tss));
    if (!tss) {
        kprint("[[[PANIC]]] COULD NOT ALLOCATE TSS\n");
        for (;;) {
            asm ("hlt");
        }
    }

    memset(tss, 0x00, sizeof(struct tss));
    // Load Interrupt Stack Table later
}