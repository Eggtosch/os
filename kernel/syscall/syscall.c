#include <syscall/syscall.h>
#include <syscall/gdt.h>
#include <interrupts/interrupts.h>
#include <common.h>
#include <debug.h>
#include <io/stdio.h>
#include <vfs/vfs.h>


static void syscall_handle(struct cpu_state *cpu_state) {
	debug(DEBUG_INFO, "syscall %#x", cpu_state->rax);
	if (cpu_state->rax == 0x0) {
		printf("%#x", cpu_state->rbx);
	} else if (cpu_state->rax == 0x1) {
		int fd       = (int)   cpu_state->rbx;
		void *buffer = (void*) cpu_state->rcx;
		u64 count    =         cpu_state->rdx;
		struct io_device *stream = vfs_get(fd);
		if (stream == NULL) {
			cpu_state->rax = -1;
			return;
		}
		cpu_state->rax = stream->read(stream, buffer, count);
		return;
	}
}


void syscall_init(void *kernel_stack) {
	gdt_init(kernel_stack);
	interrupt_register(INT_SYSCALL, syscall_handle, INT_USER);
	debug(DEBUG_INFO, "Initialized syscalls");
}
