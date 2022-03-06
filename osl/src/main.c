#include <common.h>

static int a = 0x1234;

void _start(const char *program, u64 progsize) {
	u64 rsp;
	asm volatile("mov %%rsp, %0" : "=m"(rsp) :: "memory");
	asm volatile("int $0x80" :: "a"(rsp) : );
	while (1);
}
