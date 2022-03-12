#include <stdarg.h>

#include "panic.h"


void __panic(const char* location, const char* msg, ...) {
    printk("panic at %s: ", location);

    va_list args;
    va_start(args, msg);
    vprintk(msg, args);
    va_end(args);

    printk(".\n");

    for (;;) {
        asm volatile ("hlt");
    }
}
