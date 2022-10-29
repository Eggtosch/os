#include <io/io.h>


void io_outb(u16 port, u8 value) {
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

u8 io_inb(u16 port) {
	u8 ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

u16 io_inw(u16 port) {
	u16 ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
		return ret;
}

u8 io_outinb(u16 port_out, u8 value_out, u16 port_in) {
	io_outb(port_out, value_out);
	return io_inb(port_in);
}

void io_wait(void) {
	io_inb(0x80);
}

u64 rdmsr(u32 msr) {
	u32 edx, eax;
	asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr) : "memory");
	return ((u64) edx << 32) | eax;
}

void wrmsr(u32 msr, u64 value) {
	u32 eax = value & 0xffffffff;
	u32 edx = value >> 32;
	asm volatile("wrmsr" :: "a"(eax), "d"(edx), "c"(msr) : "memory");
}

