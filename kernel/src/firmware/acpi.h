#ifndef ACPI_H
#define ACPI_H

#include <limine.h>
#include <stdint.h>
#include <stdbool.h>
#include "mm/vmm.h"

#define CHECKSUM_VALIDATE_SIZE 20
#define SDT_SIG_LEN 4

struct rsdp_t {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
}__attribute__((packed));

struct acpi_sdt_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
}__attribute__((packed));

struct rsdt_t {
    struct acpi_sdt_header header;
    uint32_t entries[];
}__attribute__((packed));

struct madt_tbl_entry {
    uint8_t type;
    uint8_t length;
}__attribute__((packed));

struct madt_t {
    struct acpi_sdt_header header;
    uint32_t lapic_addr;
    uint32_t flags;
    struct madt_tbl_entry_t* table;
}__attribute((packed));

void acpi_init();

#endif