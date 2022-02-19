#include "vga.h"

static const u64 VGA_WIDTH = 80;
static const u64 VGA_HEIGHT = 25;

static u64 vga_row;
static u64 vga_column;
static volatile u16* vga_buffer;

void vga_initialize(void) {
    vga_row = 0;
    vga_column = 0;
    u8 color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (u16*)0xB8000;
    for (u64 y = 0; y < VGA_HEIGHT; y++) {
        for (u64 x = 0; x < VGA_WIDTH; x++) {
            const u64 index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', color);
        }
    }
}

void vga_putentryat(char c, u8 color, u64 x, u64 y) {
    const u64 index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

void vga_putchar_color(char c, u8 color) {
    switch (c) {
    case '\n':
        vga_row++;
        vga_column = 0;
        if (vga_row == VGA_HEIGHT) {
            vga_row = 0;
        }
        break;

    case '\r':
        vga_column = 0;
        break;

    default:
        vga_putentryat(c, color, vga_column, vga_row);
        vga_column++;
        if (vga_column == VGA_WIDTH) {
            vga_column = 0;
            vga_row++;
            if (vga_row == VGA_HEIGHT) {
                vga_row = 0;
            }
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
