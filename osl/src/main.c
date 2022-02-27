#include <common.h>

static int a = 0x1234;

void _start(void) {
	asm volatile("int $0x80" :: "a"(a) : );
	while (1);
}
