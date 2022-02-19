#include "vga.h"

void kernel_main() {
    vga_initialize();
    vga_writestring_color("Hello, world!", VGA_COLOR_RED);
}
