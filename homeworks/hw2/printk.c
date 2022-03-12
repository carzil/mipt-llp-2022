#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "vga.h"

enum { PRINTK_BUF_SIZE = 11 };
static const char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

size_t bprintu64(char* buf, uint64_t a, int base) {
    size_t i;
    size_t bytes;

    if (a == 0) {
        buf[0] = '0';
        return 1;
    }
    for (bytes = 0; a > 0; bytes++) {
        buf[bytes] = digits[a % base];
        a /= base;
    }
    for (i = 0; i < bytes / 2; i++) {
        buf[i] ^= buf[bytes - i - 1];
        buf[bytes - i - 1] ^= buf[i];
        buf[i] ^= buf[bytes - i - 1];
    }
    return bytes;
}

size_t bprints64(char* buf, int64_t a, int base) {
    if (a < 0) {
        *(buf++) = '-';
        return 1 + bprintu64(buf, -a, base);
    }
    return bprintu64(buf, a, base);
}

size_t bprintstr(char* buf, const char* str) {
    const char* cur = str;
    size_t length = 0;
    while (*cur) {
        buf[length++] = *(cur++);
    }
    return length;
}

size_t bprintptr(char* buf, void* ptr) {
    size_t bytes;
    if (ptr == NULL) {
        return bprintstr(buf, "(nil)");
    }
    bytes = bprintstr(buf, "0x");
    return bytes + bprintu64(buf + bytes, (uint64_t) ptr, 16);
}

void vprintk(const char* fmt, va_list args) {
    const char* cursor = fmt;
    int idle = 1;
    uint32_t u32value;
    int32_t s32value;
    void* ptrvalue;
    char* str;
    char buf[PRINTK_BUF_SIZE];
    size_t size;

    uint16_t color = vga_entry(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    while (*cursor) {
        if (idle) {
            if (*cursor != '%') {
                vga_write(cursor, 1, color);
            } else {
                idle = 0;
            }
        } else {
            switch (*cursor) {
            case 'u':
                u32value = va_arg(args, uint32_t);
                size = bprintu64(buf, u32value, 10);
                break;
            case 'x':
                u32value = va_arg(args, uint32_t);
                size = bprintu64(buf, u32value, 16);
                break;
            case 'd':
                s32value = va_arg(args, int32_t);
                size = bprints64(buf, s32value, 10);
                break;
            case 'p':
                ptrvalue = va_arg(args, void*);
                size = bprintptr(buf, ptrvalue);
                break;
            case 's':
                str = va_arg(args, char*);
                vga_writestring_color(str, color);
                size = 0;
                break;
            default:
                size = 0;
                break;
            }
            if (size) {
                vga_write(buf, size, color);
            }
            idle = 1;
        }
        ++cursor;
    }
}

void printk(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);
}
