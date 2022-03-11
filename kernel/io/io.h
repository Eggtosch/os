#pragma once

#include <common.h>


void io_outb(u16 port, u8 value);
u8   io_inb(u16 port);
void io_wait(void);

u64  rdmsr(u32 msr);
void wrmsr(u32 msr, u64 value);
bool cpuid(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx);
