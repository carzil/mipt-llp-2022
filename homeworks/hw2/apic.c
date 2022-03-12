#include "apic.h"
#include "panic.h"
#include "paging.h"

#define TYPE_LAPIC          0
#define TYPE_IOAPIC         1
#define TYPE_ISO            2
#define TYPE_NMI            3
#define TYPE_LAPIC_OVERRIDE 4

#define FLAGS_ACTIVE_LOW      2
#define FLAGS_LEVEL_TRIGGERED 8

struct ioapic {
    uint32_t reg;
    uint32_t pad[3];
    uint32_t data;
};

volatile uint32_t* lapic_ptr = NULL;
volatile struct ioapic* ioapic_ptr = NULL;

struct madt_entry {
    uint8_t type;
    uint8_t length;
    uint8_t data[0];
} __attribute__((packed));

struct madt_header {
    struct acpi_sdt_header acpi;
    uint32_t lapic_addr;
    uint32_t flags;
    struct madt_entry first_entry;
} __attribute__((packed));

static void lapic_write(size_t idx, uint32_t value) {
    lapic_ptr[idx / 4] = value;
    lapic_ptr[0];
}

static uint32_t lapic_read(size_t idx) {
    return lapic_ptr[idx / 4];
}

#define APIC_ID          0x20
#define APIC_VER         0x30
#define APIC_TASKPRIOR   0x80
#define APIC_EOI         0x0B0
#define APIC_LDR         0x0D0
#define APIC_DFR         0x0E0
#define APIC_SPURIOUS    0x0F0
#define APIC_ESR         0x280
#define APIC_ICRL        0x300
#define APIC_ICRH        0x310
#define APIC_LVT_TMR     0x320
#define APIC_LVT_PERF    0x340
#define APIC_LVT_LINT0   0x350
#define APIC_LVT_LINT1   0x360
#define APIC_LVT_ERR     0x370
#define APIC_TMRINITCNT  0x380
#define APIC_TMRCURRCNT  0x390
#define APIC_TMRDIV      0x3E0
#define APIC_LAST        0x38F
#define APIC_DISABLE     0x10000
#define APIC_SW_ENABLE   0x100
#define APIC_CPUFOCUS    0x200
#define APIC_NMI         (4<<8)
#define APIC_INIT        0x500
#define APIC_BCAST       0x80000
#define APIC_LEVEL       0x8000
#define APIC_DELIVS      0x1000
#define TMR_PERIODIC     0x20000
#define TMR_BASEDIV      (1<<20)

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

#define IOAPIC_REG_TABLE  0x10

static void ioapic_write(int reg, uint32_t data) {
    ioapic_ptr->reg = reg;
    ioapic_ptr->data = data;
}

static void ioapic_enable(int irq, int target_irq) {
    ioapic_write(IOAPIC_REG_TABLE + 2 * irq, target_irq);
    ioapic_write(IOAPIC_REG_TABLE + 2 * irq + 1, 0);
}

void apic_init() {
    // Find Multiple APIC Description Table, it contains addresses of I/O APIC and LAPIC.
    struct madt_header* header = (struct madt_header*)acpi_lookup_rsdt("APIC");
    if (!header) {
        panic("MADT not found!");
    }

    lapic_ptr = (volatile uint32_t*)header->lapic_addr;

    struct madt_entry* entry = &header->first_entry;

    for (;;) {
        if ((uint8_t*)entry >= (uint8_t*)header + header->acpi.length)  {
            break;
        }

        switch (entry->type) {
            case TYPE_LAPIC:
                break;
            case TYPE_IOAPIC:
                ioapic_ptr = (volatile struct ioapic*)(*(uint32_t*)(&entry->data[2]));
                break;
        }

        entry = (struct madt_entry*)((uint8_t*)entry + entry->length);
    }

    if (!ioapic_ptr) {
        panic("cannot locate I/O APIC address");
    }

    if (!lapic_ptr) {
        panic("cannot locate Local APIC address");
    }

    // Disable old PIC.
    outb(0x20 + 1, 0xFF);
    outb(0xA0 + 1, 0xFF);

    // Enable APIC, by setting spurious interrupt vector and APIC Software Enabled/Disabled flag.
    lapic_write(APIC_SPURIOUS, 39 | APIC_SW_ENABLE);

    // Disable performance monitoring counters.
    lapic_write(APIC_LVT_PERF, APIC_DISABLE);

    // Disable local interrupt pins.
    lapic_write(APIC_LVT_LINT0, APIC_DISABLE);
    lapic_write(APIC_LVT_LINT1, APIC_DISABLE);

    // Signal EOI.
    lapic_write(APIC_EOI, 0);

    // Set highest priority for current task.
    lapic_write(APIC_TASKPRIOR, 0);

    // TODO: calibrate APIC timer to fire interrupt every millisecond.

    // APIC timer setup.
    // Divide Configuration Registers, set to X1
    lapic_write(APIC_TMRDIV, 0xB);
    // Interrupt vector and timer mode.
    lapic_write(APIC_LVT_TMR, 32 | TMR_PERIODIC);
    // Init counter.
    lapic_write(APIC_TMRINITCNT, 10000000);
}

void apic_eoi() {
    lapic_write(APIC_EOI, 0);
}
