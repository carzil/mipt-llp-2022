#pragma once

#define GDT_PRESENT     (1<<15)
#define GDT_SYSTEM      (1<<12)
#define GDT_GRANULARITY (1<<23)
#define GDT_LONG        (1<<21)
#define GDT_READ        (1<<9)
#define GDT_DPL_RING3   (3<<13)
#define GDT_CODE_SEG    (1<<11)

#define KERNEL_CODE_SEG 1
#define KERNEL_DATA_SEG 2
#define USER_DATA_SEG   3
#define USER_CODE_SEG   4
#define TSS_SEG         5

#define RPL_RING3 3
#define RPL_RING0 0

#define GDT_SEGMENT_SELECTOR(idx, rpl) (((idx) << 3) | (rpl))
