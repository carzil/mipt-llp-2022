#include <stdarg.h>

#include "panic.h"


void __panic(const char* location, const char* msg, ...) {
    // TODO: panic function should print all arguments passed via variadic parameters and then enter inifinite non-busy loop.
    // Example of a such loop could be found in boot.S.
}
