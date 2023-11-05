#include "acpi.h"
#include "klibc/io.h"
#include "mm/mm.h"
#include "klibc/memory.h"
#include "klibc/string.h"
#include "mm/vmm.h"

extern uint64_t* root_page_dir;

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

static bool sdt_validate_sig(struct acpi_sdt_header* header, const char* sig) {
    if (!memcmp(header->signature, sig, SDT_SIG_LEN)) {
        return true;
    }
    return false;
}

static bool sdt_validate_checksum(struct acpi_sdt_header* header) {
    uint8_t sum = 0;
    uint8_t* ptr = (uint8_t*)header;

    for (uint32_t i = 0; i < header->length; i++) {
        sum += ptr[i];
    }

    if ((sum & 0xFF) == 0) {
        return true;
    }
    return false;
}

struct acpi_sdt_header* acpi_find_sdt_tbl(const char* sig) {
    uint64_t num_entries = (rsdt->header.length - sizeof(rsdt->header)) / (acpi_version_2 ? 8 : 4);
    struct acpi_sdt_header* entry;
    char buf[HEX_STRING_MAX];
    for (uint64_t i = 0; i < num_entries; i++) {
        entry = (struct acpi_sdt_header*)phys_to_hh(((uintptr_t)rsdt->entries[i]));
        if (sdt_validate_sig(entry, sig) && sdt_validate_checksum(entry)) {
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

    rsdp = (struct rsdp_t*)(uintptr_t)rsdp_request.response[0].address;
    if (rsdp->revision >= 2) {
        acpi_version_2 = true;
    } else {
        rsdt = (struct rsdt_t*)(uintptr_t)phys_to_hh((uintptr_t)rsdp->rsdt_addr);
        char buf[HEX_STRING_MAX];
        kprint("rsdt phys addr: ");
        kprint(ull_to_hex(buf, (uint64_t)rsdp->rsdt_addr));
        kprint("\n");
        for (uint64_t i = 0; i < rsdt->header.length; i++) {
            vmm_map_page(root_page_dir, (uint64_t)rsdp->rsdt_addr + i, (uint64_t)rsdt + i, PTE_PRESENT | PTE_READ_WRITE);
        }
        //vmm_map_page(root_page_dir, (uint64_t)rsdp->rsdt_addr, (uint64_t)rsdt, PTE_PRESENT | PTE_READ_WRITE);
        if (!sdt_validate_sig(&rsdt->header, "RSDT") || !sdt_validate_checksum(&rsdt->header)) {
            kprint("[[[PANIC]]] RSDT INVALID\n");
            for (;;) {
                asm ("hlt");
            }
        }
    }

    madt_init();
    kprint("Success\n");
}