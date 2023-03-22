#pragma once

#include <common.h>

#define MSR_APIC_BASE        0x1B
#define MSR_MTRR_CAP         0xFE
#define MSR_MTRR_PHYS_BASE_0 0x200

u8   io_inb(u16 port);
u16  io_inw(u16 port);
void io_outb(u16 port, u8 value);
void io_outw(u16 port, u16 value);
u8   io_outinb(u16 port_out, u8 value_out, u16 port_in);

void io_wait(void);

u64  rdmsr(u32 msr);
void wrmsr(u32 msr, u64 value);
