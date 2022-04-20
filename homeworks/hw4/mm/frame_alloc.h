#pragma once

#include <stddef.h>

// frames_alloc allocates continuous memory region contains at least n pages.
void* frames_alloc(size_t n);

// frame_alloc allocates single frame.
void* frame_alloc();

// frames_free frees n frames at given base address.
void frames_free(void* addr, size_t n);

// frame_free frees frame at given address.
void frame_free(void* addr);

// frame_alloc_init initializes frame allocator. Must be called after direct physical memory mapping is created.
void frame_alloc_init();
