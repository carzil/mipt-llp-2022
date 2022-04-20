#include <stdarg.h>

#include "kernel/irq.h"
#include "panic.h"
#include "arch/x86/x86.h"


_Noreturn void __panic(const char* location, const char* msg, ...) {
    irq_disable();

    printk("panic at %s: ", location);

    va_list args;
    va_start(args, msg);
    vprintk(msg, args);
    va_end(args);

    printk(".\n");

    x86_hlt_forever();
}
