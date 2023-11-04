#include "acpi.h"
#include "klibc/io.h"
#include "mm/mm.h"
#include "klibc/memory.h"
#include "klibc/string.h"

static struct rsdp_t* rsdp;
static struct rsdt_t* rsdt;
static bool acpi_version_2 = false;
static struct madt_t* madt;

static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static bool rsdp_validate_checksum(void* rsdp) {
    uint8_t sum = 0;
    uint8_t* ptr = rsdp;
    for (uint64_t i = 0; i < CHECKSUM_VALIDATE_SIZE; i++) {
        sum += ptr[i];
    }

    if ((sum & 0xFF) != 0) {
       return false; 
    }
    return true;
}

static bool sdt_validate_checksum(struct acpi_sdt_header* sdt_header) {
    int sum = 0;
    uint8_t* ptr = (uint8_t*)sdt_header;
    for (int i = 0; i < (int)sdt_header->length; i++) {
        sum += ptr[i];
    }

    if (sum % 100 == 0) {
        return true;
    }
    return false;
}

static bool sdt_validate(struct acpi_sdt_header* sdt_header, const char* sig) {
    if (sdt_validate_checksum(sdt_header)) {
        return true;
    }
    return false;
}

struct acpi_sdt_header* acpi_find_sdt_tbl(const char* sig) {
    uint64_t num_entries = (rsdt->header.length - sizeof(rsdt->header)) / (acpi_version_2 ? 8 : 4);
    struct acpi_sdt_header* entry;

    for (uint64_t i = 0; i < num_entries; i++) {
        entry = (struct acpi_sdt_header*)(uintptr_t)rsdt->entries[i];
        if (!sdt_validate(entry, sig)) {
            return (struct acpi_sdt_header*)phys_to_hh((uintptr_t)entry);
        }
    }
    return NULL;
}

static void madt_init() {
    madt = (struct madt_t*)acpi_find_sdt_tbl("APIC");
    if (!madt) {
        kprint("[[[PANIC]]] NO MADT FOUND\n");
        for (;;) {
            asm ("hlt");
        }
    }
}

void acpi_init() {
    kprint("[KERNEL] ACPI Initializing... ");
    if(!rsdp_validate_checksum(rsdp_request.response[0].address)) {
        kprint("[[[PANIC]]] ACPI RSDP INVALID\n");
        for (;;) {
            asm ("hlt");
        }
    }
    
    rsdp = (struct rsdp_t*)rsdp_request.response[0].address;
    if (rsdp->revision >= 2) {
        acpi_version_2 = true;
        kprint("ACPI Version 2 being used!\n");
    } else {
        rsdt = (struct rsdt_t*)(uintptr_t)rsdp->rsdt_addr;
        if (!sdt_validate(&rsdt->header, "RSDT")) {
            kprint("[[[PANIC]]] NO ACPI FOUND ON COMPUTER\n");
            for (;;) {
                asm ("hlt");
            }
        }
    }

    //madt_init();
    kprint("Success\n");
}