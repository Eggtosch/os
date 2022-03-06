#include <syscall/syscall.h>
#include <syscall/gdt.h>
#include <interrupts/interrupts.h>
#include <common.h>
#include <debug.h>
#include <io/stdio.h>


static void syscall_handle(struct cpu_state *cpu_state) {
	if (cpu_state->rax == 0x1) {
		char *str = (char*) cpu_state->rbx;
		u64 size  = cpu_state->rcx;
		printf("%.*s", size, str);
	} else if (cpu_state->rax == 0x2) {
		asm volatile("hlt");
	}
}


void syscall_init(void *kernel_stack) {
	gdt_init(kernel_stack);
	interrupt_register(INT_SYSCALL, syscall_handle, INT_USER);
	debug(DEBUG_INFO, "Initialized syscalls");
}
