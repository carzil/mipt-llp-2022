#pragma once

#include <stddef.h>
#include <stdint.h>

struct acpi_sdt_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemid[6];
    char oem_tableid[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

struct acpi_sdt {
    struct acpi_sdt_header header;

    union {
        uint32_t entries[0];
        char data[0];
    };

} __attribute__((packed));

void acpi_init();
struct acpi_sdt* acpi_lookup_rsdt(const char* signature);
struct acpi_sdt* acpi_lookup_sdt(struct acpi_sdt* root, const char* signature);
