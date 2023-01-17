#include <syscall/syscall.h>

#include <syscall/gdt.h>
#include <interrupts/interrupts.h>
#include <vfs/vfs.h>
#include <memory/vmm.h>
#include <io/stdio.h>
#include <process/process.h>

#include <common.h>


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

static void sys_poll(struct cpu_state *cpu_state) {}

static void sys_mmap(struct cpu_state *cpu_state) {}
static void sys_munmap(struct cpu_state *cpu_state) {}
static void sys_sharedmem(struct cpu_state *cpu_state) {}

static void sys_exec(struct cpu_state *cpu_state) {
	const char *elf = (const char*) cpu_state->rbx;
	process_create(elf);
}

static void sys_exit(struct cpu_state *cpu_state) {
	(void) cpu_state;
	struct process *p = process_get(process_current());
	vmm_set_pagedir(NULL);
	vmm_pagedir_destroy(p->pagedir);
	kloop();
}

static void sys_clock_time(struct cpu_state *cpu_state) {}
static void sys_clock_res(struct cpu_state *cpu_state) {}
static void sys_sleep(struct cpu_state *cpu_state) {}

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
};


static void syscall_handle(struct cpu_state *cpu_state) {
	u64 syscall_number = cpu_state->rax;
	if (syscall_number >= sizeof(_handlers) / sizeof(_handlers[0])) {
		cpu_state->rax = -1;
	} else {
		printf("syscall %#x\n", syscall_number);
		_handlers[syscall_number](cpu_state);
	}
}


void syscall_init(struct boot_info *boot_info) {
	gdt_init(boot_info->stack_addr);
	interrupt_register(INT_SYSCALL, syscall_handle, INT_USER);
}

