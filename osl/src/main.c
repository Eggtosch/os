#include <syscall.h>
#include <common.h>


void _start(const char *program, u64 progsize) {
	(void) program;
	(void) progsize;
	int fd = sys_open("/dev/serial", 0);
	if (fd < 0) {
		sys_exit();
	}
	// initialize x87, sse, sse2: https://wiki.osdev.org/FPU
	//float f = 4;
	char f[] = "Hello world!\n";
	sys_write(fd, (void*) &f, sizeof(f));
	sys_exit();
}

// objdump -b binary -m i386:x86-64 --adjust-vma=0x8000000000 -D bin/osl.bin
