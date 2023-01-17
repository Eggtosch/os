#include <syscall.h>
#include <common.h>

u32 f[10002];
const char msg1[] = "writing to framebuffer...\n";

void _start(const char *program, u64 progsize) {
	(void) program;
	(void) progsize;
	int fb = sys_open("/dev/fb/buffer", 0);
	int serial = sys_open("/dev/serial", 0);
	if (fb < 0 || serial < 0) {
		sys_exit();
	}

	u8 *f8 = (u8*) f;
	f8[0] = f8[2] = f8[4] = f8[6] = 0;
	f8[1] = 255;
	f8[3] = 255;
	f8[5] = 100;
	f8[7] = 100;

	for (int i = 2; i < 10002; i++) {
		f[i] = 0x80ffffff;
	}

	sys_write(serial, (void*) msg1, sizeof(msg1));

	sys_write(fb, (void*) f, 10002 * sizeof(u32));
	sys_exit();
}

