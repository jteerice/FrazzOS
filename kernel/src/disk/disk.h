#ifndef DISK_H
#define DISK_H

typedef unsigned int FRAZZOS_DISK_TYPE;

#define FRAZZOS_DISK_TYPE_REAL 0;
#define FRAZZOS_SECTOR_SIZE 512;

struct disk {
    FRAZZOS_DISK_TYPE type;
    int sector_size;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif