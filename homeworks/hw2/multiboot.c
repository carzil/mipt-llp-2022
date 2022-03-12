#include "multiboot.h"
#include "printk.h"
#include "types.h"
#include "panic.h"

extern struct multiboot_info* _multiboot_info;
static struct multiboot_mmap_tag* mmap_tag;

struct multiboot_tag* multiboot_lookup_tag(uint32_t tag_code) {
    struct multiboot_tag* tag = &_multiboot_info->first_tag;
    for (;;) {
        if (tag->type == MULTIBOOT_TAG_END) {
            return NULL;
        }

        if (tag->type == tag_code) {
            break;
        }

        // Size doesn't include padding, so fix it to be aligned.
        u32 sz = tag->size;
        if ((tag->size & 0b111) != 0) {
            sz = (sz + 8) & ~0b111;
        }

        tag = (struct multiboot_tag*)(((u8*)tag) + sz);
    }
    return tag;
}

void multiboot_init() {
    mmap_tag = (struct multiboot_mmap_tag*)multiboot_lookup_tag(MULTIBOOT_TAG_MMAP);
    if (mmap_tag == NULL) {
        panic("bootloader didn't provide memory map");
    }

    printk("parsed multiboot info at %p, total_size=%d\n", _multiboot_info, _multiboot_info->total_size);
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
