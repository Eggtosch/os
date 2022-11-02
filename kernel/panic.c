#include <panic.h>
#include <io/stdio.h>
#include <boot/boot_info.h>

static u64 panic_write_fb(struct io_device *dev, u8 *buf, u64 buflen, __unused u64 offset) {
	struct boot_info *boot_info = dev->userdata;
	boot_info->fb_print((char*) buf, buflen);
	return buflen;
}

__attribute__((noreturn)) void _panic(const char *file, const char *function, int line, const char *fmt, ...) {
	va_list args, fb_args;
	va_start(args, fmt);
	va_copy(fb_args, args);

	struct io_device fb_dev = {NULL, panic_write_fb, boot_info_get()};

	asm volatile("cli");

	printf("[%s:%s:%d] Kernel panic: ", file, function, line);
	fprintf(&fb_dev, "[%s:%s:%d] Kernel panic: ", file, function, line);
	vprintf(fmt, args);
	vfprintf(&fb_dev, fmt, fb_args);
	printf("\n");
	fprintf(&fb_dev, "\n");

	va_end(args);
	va_end(fb_args);

	while (1) {
		asm volatile("hlt");
	}
}

