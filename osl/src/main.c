#include <common.h>
#include <syscall.h>

u8 buffer[1024];

void _start(const char *program, u64 progsize) {
	volatile u64 i = 1000000000;
	while (i--);
	u64 read = sys_read(0, buffer, 1024);
	asm volatile("int $0x80" :: "a"(0), "b"(read));
	while(1);
}
