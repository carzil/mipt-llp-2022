#include "vga.h"

static const u64 VGA_WIDTH = 80;
static const u64 VGA_HEIGHT = 25;

static u64 vga_row;
static u64 vga_column;
static volatile u16* vga_buffer;

static inline u64 vga_index(u64 row, u64 col) {
    return row * VGA_WIDTH + col;
}

void vga_init(void) {
    vga_row = 0;
    vga_column = 0;
    u8 color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (u16*)0xB8000;
    for (u64 row = 0; row < VGA_HEIGHT; row++) {
        for (u64 col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[vga_index(row, col)] = vga_entry(' ', color);
        }
    }
}

void vga_putentryat(char c, u8 color, u64 row, u64 col) {
    vga_buffer[vga_index(row, col)] = vga_entry(c, color);
}

// TODO: vga_newline_and_return should support terminal scrolling.
static void vga_newline_and_return() {
    vga_row++;
    if (vga_row == VGA_HEIGHT) {
        for (u64 row = 0; row < VGA_HEIGHT - 1; row++) {
            for (u64 col = 0; col < VGA_WIDTH; col++) {
                vga_buffer[vga_index(row, col)] = vga_buffer[vga_index(row + 1, col)];
            }
        }
        for (u64 col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[vga_index(VGA_HEIGHT - 1, col)] = 0;
        }
        vga_row = VGA_HEIGHT - 1;
    }
    vga_column = 0;
}

void vga_putchar_color(char c, u8 color) {
    switch (c) {
    case '\n':
        vga_newline_and_return();
        break;

    case '\r':
        vga_column = 0;
        break;

    default:
        vga_putentryat(c, color, vga_row, vga_column);
        vga_column++;
        if (vga_column == VGA_WIDTH) {
            vga_newline_and_return();
        }
    }
}

void vga_write(const char* data, size_t size, u8 color) {
    for (u64 i = 0; i < size; i++) {
        vga_putchar_color(data[i], color);
    }
}

void vga_writestring(const char* data) {
    vga_write(data, strlen(data), vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void vga_writestring_color(const char* data, u8 color) {
    vga_write(data, strlen(data), color);
}
