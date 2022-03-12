#include <syscall.h>
#include <common.h>
#include <framebuffer/framebuffer.h>


struct fb_buffer _fb = {
	.x = 0, .y = 0,
	.width = 0xffff, .height = 0xffff
};


void _start(const char *program, u64 progsize) {
	(void) program;
	(void) progsize;
	/*fb_init(&_fb);
	for (u64 i = 0; i < 0xff; i++) {
		fb_clear(&_fb, i << 16 | i << 8 | i);
		fb_present(&_fb);
	}
	fb_deinit(&_fb);*/
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
