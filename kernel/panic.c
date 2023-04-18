#include <panic.h>
#include <io/stdio.h>
#include <boot/boot_info.h>
#include <memory/vmm.h>
#include <process/elf.h>

static u64 panic_write_fb(struct io_device *dev, u8 *buf, u64 buflen, __unused u64 offset) {
	struct boot_info *boot_info = dev->userdata;
	boot_info->fb_print((char*) buf, buflen);
	return buflen;
}

static void print_backtrace(struct io_device *fb_dev) {
	u64 *rbp;
	asm volatile("mov %%rbp, %0" : "=a"(rbp));

	int i = 0;
	while (*rbp != 0) {
		void *p = (void*) rbp[1];
		if (p == NULL) {
			break;
		}

		const char *symbol = elf_get_kernel_symbol(p);

		printf(" [%d] %p: %s\n", i, p, symbol);
		fprintf(fb_dev, " [%d] %p: %s\n", i, p, symbol);
		rbp = (u64*) *rbp;
		i++;
	}
}

__attribute__((noreturn)) void panic(const char *fmt, ...) {
	vmm_set_pagedir(NULL);

	va_list args, fb_args;
	va_start(args, fmt);
	va_copy(fb_args, args);

	struct io_device fb_dev = {NULL, panic_write_fb, NULL, boot_info_get()};

	asm volatile("cli");

	printf("---------- Kernel panic ----------\n");
	fprintf(&fb_dev, "---------- Kernel panic ----------\n");
	vprintf(fmt, args);
	vfprintf(&fb_dev, fmt, fb_args);
	printf("\n");
	fprintf(&fb_dev, "\n");

	va_end(args);
	va_end(fb_args);

	print_backtrace(&fb_dev);

	while (1) {
		asm volatile("hlt");
	}
}
