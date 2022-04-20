#pragma once

#include <stdint.h>
#include "arch/x86/arch.h"

enum {
    SYS_SLEEP = 0,
    SYS_FORK = 1,
    SYS_GETPID = 2,
    SYS_EXIT = 3,
    SYS_MAX,
};

typedef int64_t (*syscall_fn_t)(arch_regs_t*);
