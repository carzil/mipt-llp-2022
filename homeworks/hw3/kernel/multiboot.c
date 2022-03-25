#include "multiboot.h"
#include "kernel/printk.h"
#include "types.h"
#include "mm/paging.h"
#include "kernel/panic.h"

static struct multiboot_info* multiboot_info;
static struct multiboot_mmap_tag* mmap_tag;

struct multiboot_tag* multiboot_lookup_tag(uint32_t tag_code) {
    struct multiboot_tag* tag = &multiboot_info->first_tag;
    for (;;) {
        if (tag->type == MULTIBOOT_TAG_END) {
            return NULL;
        }

        if (tag->type == tag_code) {
            break;
        }

        // Size doesn't include padding, so fix it to be aligned.
        u32 sz = ALIGN_UP(tag->size, 8);

        tag = (struct multiboot_tag*)(((u8*)tag) + sz);
    }
    return tag;
}

void multiboot_init(struct multiboot_info* info) {
    multiboot_info = info;
    mmap_tag = (struct multiboot_mmap_tag*)multiboot_lookup_tag(MULTIBOOT_TAG_MMAP);
    if (mmap_tag == NULL) {
        panic("bootloader didn't provide memory map");
    }

    printk("parsed multiboot info at %p, total_size=%d\n", multiboot_info, multiboot_info->total_size);
}

void multiboot_mmap_iter_init(struct multiboot_mmap_iter* it) {
    it->curr = NULL;
}

struct multiboot_mmap_entry* multiboot_mmap_iter_next(struct multiboot_mmap_iter* it) {
    if (it->curr == NULL) {
        it->curr = &mmap_tag->first_entry;
    } else {
        it->curr = (struct multiboot_mmap_entry*)((u8*)it->curr + mmap_tag->entry_size);
    }

    if ((u8*)it->curr - (u8*)mmap_tag >= mmap_tag->base.size) {
        return NULL;
    }

    return it->curr;
}

mem_region_t multiboot_mem_region() {
    mem_region_t res = {
        .start = multiboot_info,
        .end   = multiboot_info + ALIGN_UP(multiboot_info->total_size, PAGE_SIZE),
    };
    return res;
}
