#pragma once

#include <io/stdio.h>

#define kexit() printf("kexit();\n"); do { asm volatile("cli\n\thlt"); } while (1)

