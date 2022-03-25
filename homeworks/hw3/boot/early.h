#pragma once

#include "types.h"
#include "kernel/multiboot.h"

typedef struct early_data {
    // Contains virtual address of multiboot info.
    struct multiboot_info* multiboot_info;
} early_data_t;
