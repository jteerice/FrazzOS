#include "disk.h"
#include "../devices/cpu.h"
#include "klibc/io.h"
#include "klibc/memory.h"
#include "status.h"

extern uint16_t inw(uint16_t port);

struct disk disk;

static int disk_read_sector(int lba, int total, void* buf) {

    outb(0x1f6, (lba >> 24) | 0xe0);
    outb(0x1f2, total);
    outb(0x1f3, (unsigned char)(lba & 0xff));
    outb(0x1f4, (unsigned char)(lba >> 8));
    outb(0x1f5, (unsigned char)(lba >> 16));
    outb(0x1f7, 0x20);

    unsigned short* ptr = (unsigned short*)buf;
    for (int i = 0; i < total; i++) {
        char c = inb(0x1f7);

        while(!(c & 0x08)) {
            c = inb(0x1f7);
        }

        for (int i = 0; i < 256; i++) {
            *ptr = inw(0x1f0);
            ptr++;
        }
    }

    return 0;
}

// Have this function in case we want more disks one day
struct disk* disk_get(int index) {
    if (index != 0) {
        return NULL;
    }

    return &disk;
}

void disk_search_and_init() {
    kprint("[KERNEL] Initializing disk... ");
    memset(&disk, 0, sizeof(disk));
    disk.type = FRAZZOS_DISK_TYPE_REAL;
    disk.sector_size = FRAZZOS_SECTOR_SIZE;
    kprint("Success\n");
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf) {
    if (idisk != &disk) {
        return EINVARG;
    }
    return disk_read_sector(lba, total, buf);
}