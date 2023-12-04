#ifndef ACPI_H
#define ACPI_H

#include <limine.h>
#include <stdint.h>
#include <stdbool.h>
#include "mm/vmm.h"

#define CHECKSUM_VALIDATE_SIZE 20
#define SDT_SIG_LEN 4
#define ACPI_APIC_TBL_SIZE 256

#define PROCESSOR_LOCAL_APIC 0
#define IO_APIC 1
#define IO_APIC_INTERRUPT_SRC_OVERRIDE 2
#define IO_APIC_NMI_SRC 3
#define LOCAL_APIC_NMI 4
#define LOCAL_APIC_ADDR_OVERRIDE 5
#define PROCESSOR_LOCAL_X2_APIC 9

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

struct madt_tbl_entry_t {
    uint8_t type;
    uint8_t length;
}__attribute__((packed));

struct madt_t {
    struct acpi_sdt_header header;
    uint32_t lapic_addr;
    uint32_t flags;
    struct madt_tbl_entry_t table[];
}__attribute__((packed));

struct proc_lapic_t {
    struct madt_tbl_entry_t record;
    uint8_t proc_id;
    uint8_t apic_id;
    uint32_t flags;
}__attribute__((packed));

struct io_apic_t {
    struct madt_tbl_entry_t record;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t gsib;
}__attribute__((packed));

struct ioso_apic_t {
    struct madt_tbl_entry_t record;
    uint8_t bus;
    uint8_t irq;
    uint32_t gsi;
    short flags;
}__attribute__((packed));

struct ionmi_apic_t {
    struct madt_tbl_entry_t record;
    uint8_t nmi;
    uint8_t reserved;
    short flags;
    uint32_t gsi;
}__attribute__((packed));

struct nmi_lapic_t {
    struct madt_tbl_entry_t record;
    uint8_t proc_id;
    short flags;
    uint8_t lint;
}__attribute__((packed));

struct addro_lapic_t {
    struct madt_tbl_entry_t record;
    short reserved;
    uint64_t lapic_addr;
}__attribute__((packed));

struct procx2_lapic_t {
    struct madt_tbl_entry_t record;
    short reserved;
    uint32_t x2_lapic_id;
    uint32_t flags;
    uint32_t apic_id;
}__attribute__((packed));

struct hpet_address {
    uint8_t address_id;
    uint8_t register_width;
    uint8_t register_offset;
    uint8_t reserved;
    uint64_t address;
}__attribute__((packed));

struct hpet_t {
    struct acpi_sdt_header header;
    uint8_t hardware_id;
    uint8_t comparator_cnt : 5;
    uint8_t counter_size : 1;
    uint8_t resserved : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    struct hpet_address address_info;
    uint8_t hpet_num;
    uint16_t minimum_tick;
    uint8_t page_prot;
}__attribute__((packed));

void acpi_init();
struct acpi_sdt_header* acpi_find_sdt_tbl(const char* sig);

#endif