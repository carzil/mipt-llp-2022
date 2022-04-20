#pragma once

#include <stddef.h>

#include "arch/x86/arch.h"
#include "mm/vmem.h"

#define MAX_TASK_COUNT (1 << 16)

typedef enum state {
    TASK_NOT_ALLOCATED = 0,
    TASK_RUNNABLE      = 1,
    TASK_WAITING       = 2,
    TASK_ZOMBIE        = 3,
} state_t;

typedef struct task {
    // arch_thread_t must be the first member.
    arch_thread_t arch_thread;
    size_t pid;
    state_t state;
    uint64_t flags;
    int ticks;
    vmem_t vmem;
    int exitcode;
} task_t;

void sched_start();
void sched_switch();
void sched_timer_tick();

extern task_t* _current;
#define sched_current() _current
