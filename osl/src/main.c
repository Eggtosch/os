#include <common.h>

static int a = 0x1234;

void _start(void) {
	//asm volatile("div %0" :: "r"((u16) 0) : );
	asm volatile("int $0x80" :: "a"(a) : );
}
