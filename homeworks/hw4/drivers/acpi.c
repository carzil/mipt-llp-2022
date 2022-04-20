#include "acpi.h"
#include "common.h"
#include "kernel/panic.h"
#include "kernel/multiboot.h"
#include "mm/paging.h"

struct acpi_sdt* acpi_lookup_sdt(struct acpi_sdt* root, const char* signature) {
    size_t sz = (root->header.length - sizeof(root->header)) / 4;
    for (size_t i = 0; i < sz; i++) {
        struct acpi_sdt* sdt = PHYS_TO_VIRT((void*)(uint64_t)root->entries[i]);
        if (memcmp(signature, &sdt->header.signature, 4) == 0) {
            return sdt;
        }
    }
    return NULL;
}

static struct acpi_sdt* rsdt = NULL;

struct acpi_sdt* acpi_lookup_rsdt(const char* signature) {
    return acpi_lookup_sdt(rsdt, signature);
}

struct acpi_rsdp {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_addr;
} __attribute__ ((packed));


void acpi_init() {
    struct multiboot_tag* mb_tag = multiboot_lookup_tag(MULTIBOOT_TAG_ACPI_OLD_RSDP);
    if (mb_tag == NULL) {
        panic("no multiboot tag with ACPI RSDP addres");
    }
    struct acpi_rsdp* rsdp = (void*)(uint64_t)&mb_tag->data;
    rsdt = PHYS_TO_VIRT((uint64_t)rsdp->rsdt_addr);
    printk("found RSDT at %p\n", rsdt);

    // TODO: verify ACPI table checksums here.

}
