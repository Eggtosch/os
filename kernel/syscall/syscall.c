#include <syscall/syscall.h>
#include <syscall/gdt.h>
#include <interrupts/interrupts.h>
#include <common.h>
#include <debug.h>


static void syscall_handle(struct cpu_state *cpu_state) {
	debug(DEBUG_INFO, "got syscall: rax = %#x", cpu_state->rax);
}


void syscall_init(void *kernel_stack) {
	gdt_init(kernel_stack);
	interrupt_register(INT_SYSCALL, syscall_handle);
	debug(DEBUG_INFO, "Initialized syscalls");
}
