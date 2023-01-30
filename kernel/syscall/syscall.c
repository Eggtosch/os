#include <syscall/syscall.h>

#include <syscall/gdt.h>
#include <interrupts/interrupts.h>

#include <vfs/vfs.h>

#include <memory/vmm.h>

#include <process/process.h>

#include <driver/util.h>
#include <acpi/hpet.h>

#include <framebuffer/framebuffer.h>


static void sys_open(struct cpu_state *cpu_state) {
	const char *name = (const char*) cpu_state->rbx;
	cpu_state->rax = vfs_open(name);
}

static void sys_read(struct cpu_state *cpu_state) {
	int fd       = (int)   cpu_state->rbx;
	void *buffer = (void*) cpu_state->rcx;
	u64 count    =         cpu_state->rdx;
	u64 offset   =         cpu_state->rdi;
	struct io_device *stream = vfs_get(fd);
	if (stream == NULL) {
		cpu_state->rax = -1;
	} else {
		cpu_state->rax = stream->read(stream, buffer, count, offset);
	}
}

static void sys_write(struct cpu_state *cpu_state) {
	int fd       = (int)   cpu_state->rbx;
	void *buffer = (void*) cpu_state->rcx;
	u64 count    =         cpu_state->rdx;
	u64 offset   =         cpu_state->rdi;
	struct io_device *stream = vfs_get(fd);
	if (stream == NULL) {
		cpu_state->rax = -1;
	} else {
		cpu_state->rax = stream->write(stream, buffer, count, offset);
	}
}

static void sys_poll(__unused struct cpu_state *cpu_state) {}

static void sys_mmap(__unused struct cpu_state *cpu_state) {}
static void sys_munmap(__unused struct cpu_state *cpu_state) {}
static void sys_sharedmem(__unused struct cpu_state *cpu_state) {}

static void sys_exec(struct cpu_state *cpu_state) {
	const char *elf = (const char*) cpu_state->rbx;
	pid_t new_proc = process_create(elf);
	cpu_state->rax = new_proc;
}

static void sys_exit(struct cpu_state *cpu_state) {
	(void) cpu_state;
	struct process *p = process_get(process_current());

	p->status = PROC_NONE;

	if (p->read_pipe.close != NULL) {
		p->read_pipe.close(&p->read_pipe);
	}
	if (p->write_pipe.close != NULL) {
		p->write_pipe.close(&p->write_pipe);
	}

	for (int i = 0; i < 8; i++) {
		if (p->fds[i] != NULL && p->fds[i]->close != NULL) {
			p->fds[i]->close(p->fds[i]);
		}
	}

	vmm_set_pagedir(NULL);
	vmm_pagedir_destroy(p->pagedir);

	kloop();
}

static void sys_clock_time(struct cpu_state *cpu_state) {
	cpu_state->rax = time();
	cpu_state->rbx = hpet_current_ns();
}

static void sys_clock_res(struct cpu_state *cpu_state) {
	cpu_state->rax = 0;
	cpu_state->rbx = hpet_precision_ns();
}

static void sys_sleep(__unused struct cpu_state *cpu_state) {}

static void sys_fb_info(struct cpu_state *cpu_state) {
	struct boot_info *boot_info = boot_info_get();
	struct fb_info *fb_info = &boot_info->fb_info;
	cpu_state->rax = (fb_info->fb_width << 16) | fb_info->fb_height;
}

static void sys_fb_read(struct cpu_state *cpu_state) {
	u16 x = (cpu_state->rbx >> 48) & 0xffff;
	u16 y = (cpu_state->rbx >> 32) & 0xffff;
	u16 width = (cpu_state->rbx >> 16) & 0xffff;
	u16 height = cpu_state->rbx & 0xffff;
	u32 *buffer = (u32*) cpu_state->rcx;
	cpu_state->rax = framebuffer_read(x, y, width, height, buffer);
}

static void sys_fb_write(struct cpu_state *cpu_state) {
	u16 x = (cpu_state->rbx >> 48) & 0xffff;
	u16 y = (cpu_state->rbx >> 32) & 0xffff;
	u16 width = (cpu_state->rbx >> 16) & 0xffff;
	u16 height = cpu_state->rbx & 0xffff;
	u32 *buffer = (u32*) cpu_state->rcx;
	cpu_state->rax = framebuffer_write(x, y, width, height, buffer);
}

void (*_handlers[])(struct cpu_state*) = {
	// file system calls
	sys_open, sys_read, sys_write, sys_poll,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	// memory system calls
	sys_mmap, sys_munmap, sys_sharedmem, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	// process management system calls
	sys_exec, sys_exit, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	// time system calls
	sys_clock_time, sys_clock_res, sys_sleep, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	sys_fb_info, sys_fb_read, sys_fb_write, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
};


static void syscall_handle(struct cpu_state *cpu_state) {
	u64 syscall_number = cpu_state->rax;
	if (syscall_number >= sizeof(_handlers) / sizeof(_handlers[0])) {
		cpu_state->rax = -1;
	} else {
		_handlers[syscall_number](cpu_state);
	}
}


void syscall_init(struct boot_info *boot_info) {
	gdt_init(boot_info->stack_addr);
	interrupt_register(INT_SYSCALL, syscall_handle, INT_USER);
}

