#include <syscall/syscall.h>

#include <syscall/gdt.h>
#include <interrupts/interrupts.h>
#include <vfs/vfs.h>
#include <memory/vmm.h>
#include <io/stdio.h>
#include <process/process.h>

#include <common.h>


static u64 sys_open(struct cpu_state *cpu_state) {
	const char *name = (const char*) cpu_state->rbx;
	u64 flags        =               cpu_state->rcx;
	(void) flags;
	int fd = vfs_open(name);
	return fd;
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
	int fd       = (int)   cpu_state->rbx;
	void *buffer = (void*) cpu_state->rcx;
	u64 count    =         cpu_state->rdx;
	struct io_device *stream = vfs_get(fd);
	if (stream == NULL) {
		return -1;
	}
	return stream->write(stream, buffer, count);
}

static u64 sys_mmap(struct cpu_state *cpu_state) {
	return cpu_state->rax;
}

static u64 sys_munmap(struct cpu_state *cpu_state) {
	return cpu_state->rax;
}

static u64 sys_exit(struct cpu_state *cpu_state) {
	struct process *p = process_get(process_current());
	vmm_set_pagedir(NULL);
	vmm_pagedir_destroy(p->pagedir);
	while (1) {
		asm volatile("hlt");
	}
	return cpu_state->rax;
}


u64 (*_handlers[])(struct cpu_state*) = {
	sys_open,
	sys_read,
	sys_write,
	sys_mmap,
	sys_munmap,
	sys_exit
};


static void syscall_handle(struct cpu_state *cpu_state) {
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
}

