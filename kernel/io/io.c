#include <io/io.h>


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

bool cpuid(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx) {
	u32 cpuid_max;
	asm volatile("cpuid" : "=a"(cpuid_max) : "a"(leaf & 0x80000000) :
						   "ebx", "ecx", "edx");
	if (leaf > cpuid_max)
		return false;
	asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) :
						   "a"(leaf), "c"(subleaf));
	return true;
}
