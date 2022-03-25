#pragma once

#include <stddef.h>

#include "common.h"
#include "defs.h"

typedef struct obj_alloc {
    // TODO: fill me.
} obj_alloc_t;

#define OBJ_ALLOC_DEFINE(var, type) obj_alloc_t var = { /* initialize fields here, hint: use sizeof(type) */ }
#define OBJ_ALLOC_DECLARE(var) obj_alloc_t var

// object_alloc allocates single object and returns its virtual address.
void* object_alloc(obj_alloc_t* alloc);

// object_free frees obj associated with given allocator.
void object_free(obj_alloc_t* alloc, void* obj);
