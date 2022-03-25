#pragma once

#include <stdarg.h>

#include "printk.h"


#define S1(x) #x
#define S2(x) S1(x)

void __panic(const char* location, const char* msg, ...);

#define panic(msg, ...) __panic(__FILE__ ":" S2(__LINE__), msg __VA_OPT__(,) __VA_ARGS__)

#define BUG_ON(expr) if (expr) { panic("bug: '" #expr "'"); }
#define BUG_ON_NULL(expr) BUG_ON(expr == NULL)
