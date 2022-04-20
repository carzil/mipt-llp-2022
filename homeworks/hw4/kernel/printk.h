// Original implementation by @pixelindigo: https://github.com/carzil/KeltOS/blob/master/kernel/printk.c

#pragma once
#include <stdarg.h>

void printk(const char* fmt, ...);
void vprintk(const char* fmt, va_list args);
