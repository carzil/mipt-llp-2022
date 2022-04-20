#pragma once

#include <stdint.h>

typedef struct {
    uint64_t rsp;
} context_t;

typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t ret_addr;
} on_stack_context_t;

void context_switch(context_t* curr, context_t* next);
