#pragma once

#define kexit() do { asm volatile("cli\n\thlt"); } while (1)
