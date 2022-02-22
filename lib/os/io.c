#include <os/io.h>

void io_outb(u16 port, u8 value) {
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

u8 io_inb(u16 port) {
	u8 ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void io_wait(void) {
	io_inb(0x80);
}
