#include <syscall.h>
#include <common.h>

u32 f[10000];
const char msg1[] = "writing to framebuffer...\n";

void _start(const char *program, u64 progsize) {
	(void) program;
	(void) progsize;
	int serial = sys_open("/dev/serial", 0);
	if (serial < 0) {
		sys_exit();
	}

	for (int i = 0; i < 10000; i++) {
		f[i] = 0x80ffffff;
	}

	sys_write(serial, (void*) msg1, sizeof(msg1));
	sys_fb_write(100, 100, 100, 100, f);

	sys_exit();
}
