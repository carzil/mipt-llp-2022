#include "vga.h"
#include "multiboot.h"
#include "printk.h"
#include "panic.h"
#include "irq.h"
#include "acpi.h"
#include "apic.h"

void dump_mmap() {
    struct multiboot_mmap_iter mmap_iter;
    multiboot_mmap_iter_init(&mmap_iter);

    struct multiboot_mmap_entry* mmap_entry;
    while ((mmap_entry = multiboot_mmap_iter_next(&mmap_iter)) != NULL) {
        printk("mem region: ");
        switch (mmap_entry->type) {
        case MULTIBOOT_MMAP_TYPE_RAM:
            printk("[RAM]      ");
            break;
        case MULTIBOOT_MMAP_TYPE_ACPI:
            printk("[ACPI]     ");
            break;
        default:
            printk("[RESERVED] ");
            break;
        }
        printk(" base_addr=%p, len=%d\n", mmap_entry->base_addr, mmap_entry->length);
    }
}

void kernel_main() {
    irq_init();
    vga_init();
    multiboot_init();
    acpi_init();
    apic_init();

    printk("HeLL OS loaded.\n");
    dump_mmap();

    irq_enable();
}
