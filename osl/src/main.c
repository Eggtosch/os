#include <syscall.h>
#include <common.h>

char f[] = "Hello world, this is a hello world string :()!\n";
__attribute__((used)) static int g[100];

void _start(const char *program, u64 progsize) {
	(void) program;
	(void) progsize;
	int fd = sys_open("/dev/serial", 0);
	if (fd < 0) {
		sys_exit();
	}
	// initialize x87, sse, sse2: https://wiki.osdev.org/FPU
	//float f = 4;
	//char f[] = "Hello world!\n";
	sys_write(fd, (void*) &f, sizeof(f));
	while (1) {
		sys_exit();
	}
}

// objdump -b binary -m i386:x86-64 --adjust-vma=0x80000000 -D bin/osl.bin

