#include <syscall/syscall.h>

#include <syscall/gdt.h>
#include <interrupts/interrupts.h>
#include <vfs/vfs.h>
#include <device/framebuffer.h>
#include <memory/vmm.h>
#include <io/stdio.h>

#include <debug.h>
#include <common.h>


static u64 sys_open(struct cpu_state *cpu_state) {
	printf("%#x", cpu_state->rbx);
	return cpu_state->rax;
}

static u64 sys_read(struct cpu_state *cpu_state) {
	int fd       = (int)   cpu_state->rbx;
	void *buffer = (void*) cpu_state->rcx;
	u64 count    =         cpu_state->rdx;
	struct io_device *stream = vfs_get(fd);
	if (stream == NULL) {
		return -1;
	}
	return stream->read(stream, buffer, count);
}

static u64 sys_write(struct cpu_state *cpu_state) {
	return cpu_state->rax;
}

static u64 sys_fb_init(struct cpu_state *cpu_state) {
	struct fb_buffer_user *buf = (struct fb_buffer_user*) cpu_state->rbx;
	return (u64) framebuffer_init_user(buf);
}

static u64 sys_fb_deinit(struct cpu_state *cpu_state) {
	struct fb_buffer_user *buf = (struct fb_buffer_user*) cpu_state->rbx;
	return (u64) framebuffer_deinit_user(buf);
}

static u64 sys_fb_present(struct cpu_state *cpu_state) {
	struct fb_buffer_user *buf = (struct fb_buffer_user*) cpu_state->rbx;
	return (u64) framebuffer_present_user(buf);
}

static u64 sys_mmap(struct cpu_state *cpu_state) {
	return cpu_state->rax;
}

static u64 sys_munmap(struct cpu_state *cpu_state) {
	return cpu_state->rax;
}

static u64 sys_exit(struct cpu_state *cpu_state) {
	u64 *old_pd = vmm_get_pagedir();
	vmm_set_pagedir(NULL);
	vmm_pagedir_destroy(old_pd);
	while (1) {
		asm volatile("hlt");
	}
	return cpu_state->rax;
}


u64 (*_handlers[])(struct cpu_state*) = {
	sys_open,
	sys_read,
	sys_write,
	sys_fb_init,
	sys_fb_deinit,
	sys_fb_present,
	sys_mmap,
	sys_munmap,
	sys_exit
};


static void syscall_handle(struct cpu_state *cpu_state) {
	debug(DEBUG_INFO, "syscall %#x", cpu_state->rax);
	u64 syscall_number = cpu_state->rax;
	if (syscall_number >= sizeof(_handlers) / sizeof(_handlers[0])) {
		cpu_state->rax = -1;
	} else {
		cpu_state->rax = _handlers[syscall_number](cpu_state);
	}
}


void syscall_init(void *kernel_stack) {
	gdt_init(kernel_stack);
	interrupt_register(INT_SYSCALL, syscall_handle, INT_USER);
	debug(DEBUG_INFO, "Initialized syscalls");
}
