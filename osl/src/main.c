#include <syscall.h>
#include <common.h>


void _start(const char *program, u64 progsize) {
	struct fb_buffer fb = {
		.x = 0,
		.y = 0,
		.width  = 400,
		.height = 400
	};
	sys_fb_init(&fb);
	for (u64 i = 0; i < fb.width * fb.height; i++) {
		fb.buffer[i] = 0x80ff0000;
	}
	sys_fb_present(&fb);
	sys_exit();
}
