#include "acpi.h"
#include "klibc/alloc.h"
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

static struct proc_apic_t**    proc_apic_tbl;
static struct io_apic_t**      io_apic_tbl;
static struct ioso_apic_t**    ioso_apic_tbl;
static struct ionmi_apic_t**   ionmi_apic_tbl;
static struct nmi_lapic_t**    nmi_lapic_tbl;
static struct addro_lapic_t**  addro_lapic_tbl;
static struct procx2_lapic_t** procx2_lapic_tbl;

uint64_t    proc_apic_idx = 0;
uint64_t      io_apic_idx = 0;
uint64_t    ioso_apic_idx = 0;
uint64_t   ionmi_apic_idx = 0;
uint64_t    nmi_lapic_idx = 0;
uint64_t  addro_lapic_idx = 0;
uint64_t procx2_lapic_idx = 0;

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
    for (uint64_t i = 0; i < num_entries; i++) {
        entry = (struct acpi_sdt_header*)phys_to_hh(((uintptr_t)rsdt->entries[i]));
        if (sdt_validate_sig(entry, sig) && sdt_validate_checksum(entry)) {
            return (struct acpi_sdt_header*)((uintptr_t)entry);
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

    char buf[HEX_STRING_MAX];
    proc_apic_tbl    = malloc(ACPI_APIC_TBL_SIZE);
    kprint("first tbl addr @ ");
    kprint(ull_to_hex(buf, (uint64_t)proc_apic_tbl));
    kprint("\n");
    io_apic_tbl      = malloc(ACPI_APIC_TBL_SIZE);
    kprint("second tbl addr @ ");
    kprint(ull_to_hex(buf, (uint64_t)io_apic_tbl));
    kprint("\n");
    ioso_apic_tbl    = malloc(ACPI_APIC_TBL_SIZE);
    kprint("third tbl addr @ ");
    kprint(ull_to_hex(buf, (uint64_t)ioso_apic_tbl));
    kprint("\n");
    ionmi_apic_tbl   = malloc(ACPI_APIC_TBL_SIZE);
    nmi_lapic_tbl    = malloc(ACPI_APIC_TBL_SIZE);
    addro_lapic_tbl  = malloc(ACPI_APIC_TBL_SIZE);
    procx2_lapic_tbl = malloc(ACPI_APIC_TBL_SIZE);

    uint64_t max_madt_addr = (uint64_t)(&madt->header + madt->header.length);
    kprint("madt table addr @ ");
    kprint(ull_to_hex(buf, (uint64_t)&madt->table));
    kprint("\n");
    uint8_t* ptr = (uint8_t*)&madt->table;
    kprint("ptr @ ");
    kprint(ull_to_hex(buf, (uint64_t)ptr));
    kprint("\n");
    kprint("first entry entry -- ");
    kprint(ull_to_hex(buf, (uint64_t)(*ptr & 0xFF)));
    kprint(" first entry length -- ");
    kprint(ull_to_hex(buf, (uint64_t)(*(ptr + 1) & 0xFF)));
    kprint("\n");
    while ((uint64_t)ptr < max_madt_addr) {
        kprint("here\n");
        switch(*ptr) {
            case PROCESSOR_LOCAL_APIC:
                kprint("a\n");
                proc_apic_tbl[proc_apic_idx++] = (struct proc_apic_t*)ptr;
                kprint("fucked!\n");
                break;
            case IO_APIC:
                kprint("b\n");
                io_apic_tbl[io_apic_idx++] = (struct io_apic_t*)ptr;
                break;
            case IO_APIC_INTERRUPT_SRC_OVERRIDE:
                kprint("c\n");
                ioso_apic_tbl[ioso_apic_idx++] = (struct ioso_apic_t*)ptr;
                break;
            case IO_APIC_NMI_SRC:
                kprint("d\n");
                ionmi_apic_tbl[ionmi_apic_idx++] = (struct ionmi_apic_t*)ptr;
                break;
            case LOCAL_APIC_NMI:
                kprint("e\n");
                nmi_lapic_tbl[nmi_lapic_idx++] = (struct nmi_lapic_t*)ptr;
                break;
            case LOCAL_APIC_ADDR_OVERRIDE:
                kprint("f\n");
                addro_lapic_tbl[addro_lapic_idx++] = (struct addro_lapic_t*)ptr;
                break;
            case PROCESSOR_LOCAL_X2_APIC:
                kprint("g\n");
                procx2_lapic_tbl[procx2_lapic_idx++] = (struct procx2_lapic_t*)ptr;
                break;
        }
        ptr += *(ptr + 1);
        kprint("here2\n");
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
        for (uint64_t i = 0; i < rsdt->header.length; i++) {
            vmm_map_page(root_page_dir, (uint64_t)rsdp->rsdt_addr + i, (uint64_t)rsdt + i, PTE_PRESENT | PTE_READ_WRITE);
        }
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