#pragma once

#include <common.h>

#define MSR_APIC_BASE 0x1B
#define MSR_PAT 0x277
#define MSR_TSC_DEADLINE 0x6e0

/*
 * read a byte from an io port.
 * port: io port.
 * return: the byte that was read.
 */
u8 io_inb(u16 port);

/*
 * read a 16-bit word from an io port.
 * port: io port.
 * return: the word that was read.
 */
u16 io_inw(u16 port);

/*
 * write a byte to an io port.
 * port: io port.
 * value: the byte to be written.
 */
void io_outb(u16 port, u8 value);

/*
 * write a 16-bit word to an io port.
 * port: io port.
 * value: the word to be written.
 */
void io_outw(u16 port, u16 value);

/*
 * write a byte to an io port and read a byte from another port directly after.
 * port_out: io port to write to.
 * value_out: byte to write.
 * port_in: io port to read from.
 */
u8 io_outinb(u16 port_out, u8 value_out, u16 port_in);

/*
 * read a model specific register.
 * msr: the register number.
 * return: the register value.
 */
u64 rdmsr(u32 msr);

/*
 * write to a model specific register.
 * msr: the register number.
 * value: the value to write.
 */
void wrmsr(u32 msr, u64 value);
