#include <stdbool.h>
#include "frame_alloc.h"
#include "kernel/multiboot.h"
#include "kernel/panic.h"
#include "defs.h"
#include "paging.h"
#include "linker.h"

typedef struct frame {
    struct frame* next;
} __attribute__((aligned(PAGE_SIZE))) frame_t;

static bool intersects(frame_t* frame, mem_region_t area) {
    return area.start <= (void*)frame && (void*)frame < area.end;
}

static size_t preserved_areas_size = 0;
static mem_region_t preserved_areas[100];

// mark_preserved_area marks memory area as allocated, all its frames are not touched by frame allocator.
static void mark_preserved_area(mem_region_t area) {
    area.end = (void*)ALIGN_UP(area.end, PAGE_SIZE);
    printk("preserved area: %p-%p\n", area.start, area.end);
    preserved_areas[preserved_areas_size++] = area;
}

// mark_preserved_areas fills pre-allocated regions: early & kernel sections, multiboot info, etc
void mark_preserved_areas() {
    mark_preserved_area((mem_region_t){ .start = PHYS_TO_VIRT(&_phys_start_kernel_sections), .end = PHYS_TO_VIRT(&_phys_end_kernel_sections) });
    mark_preserved_area((mem_region_t){ .start = PHYS_TO_VIRT(&_phys_start_user), .end = PHYS_TO_VIRT(&_phys_end_user) });
    mark_preserved_area(multiboot_mem_region());
}

// intersects_with_kernel_sections checks if a frame at the given virtual address intersects with kernel sections.
static bool is_allocated(frame_t* frame) {
    for (size_t i = 0; i < preserved_areas_size; i++) {
        if (intersects(frame, preserved_areas[i])) {
            return true;
        }
    }
    return false;
}

static frame_t* freelist_head;

// allocated_memory_region adds a region of RAM to the frame allocator.
static size_t frame_alloc_add_area(frame_t* base, size_t sz) {
    size_t pgcnt = 0;
    frame_t* prev_free = NULL;
    frame_t* first_free = NULL;
    for (size_t i = 0; i < sz; i++) {
        frame_t* curr = &base[i];
        if (is_allocated(curr)) {
            continue;
        }
        if (first_free == NULL) {
            prev_free = curr;
            first_free = curr;
            pgcnt++;
            continue;
        }
        prev_free->next = curr;
        prev_free = curr;
        pgcnt++;
    }
    prev_free->next = freelist_head;
    freelist_head = first_free;
    return pgcnt;
}

// frame_alloc_add_areas obtains memory areas of usable RAM from Multiboot2 and adds them to the frame allocator.
static void frame_alloc_add_areas() {
    struct multiboot_mmap_iter mmap_it;
    multiboot_mmap_iter_init(&mmap_it);
    struct multiboot_mmap_entry* mmap_entry;
    size_t pgcnt = 0;
    while ((mmap_entry = multiboot_mmap_iter_next(&mmap_it)) != NULL) {
        if (mmap_entry->type != MULTIBOOT_MMAP_TYPE_RAM) {
            continue;
        }
        pgcnt += frame_alloc_add_area(PHYS_TO_VIRT(mmap_entry->base_addr), mmap_entry->length / PAGE_SIZE);
    }
    printk("initialized page_alloc with %d pages\n", pgcnt);
}

void frame_alloc_init() {
    mark_preserved_areas();
    frame_alloc_add_areas();
}

void* frames_alloc(size_t n) {
    if (freelist_head == NULL) {
        return NULL;
    }
    BUG_ON(n != 1);
    frame_t* frame = freelist_head;
    freelist_head = frame->next;
    frame->next = NULL;
    return frame;
}

void frames_free(void* addr, size_t n) {
    BUG_ON(addr == NULL);
    BUG_ON(n != 1);

    frame_t* frame = addr;
    frame->next = freelist_head;
    freelist_head = frame;
}

void* frame_alloc() {
    return frames_alloc(1);
}

void frame_free(void* addr) {
    return frames_free(addr, 1);
}
