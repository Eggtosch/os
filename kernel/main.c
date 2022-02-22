#include <boot/boot_info.h>

#include <device/framebuffer.h>
#include <device/serial.h>
#include <console/console.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <interrupts/idt.h>
#include <syscall/syscall.h>
#include <device/keyboard.h>

#include <process/process.h>

#include <stdio.h>
#include <debug.h>

void kmain(struct boot_info *boot_info) {
	framebuffer_init(&(boot_info->fb_info));
	serial_init();
	console_init();
	stdio_init(console_io);
	debug(DEBUG_INFO, "Initialized stdio system");
	debug(DEBUG_INFO, "Initialized framebuffer");
	debug(DEBUG_INFO, "Initialized serial");
	debug(DEBUG_INFO, "Initialized console");

	pmm_init(&(boot_info->mem_info));
	vmm_init();

	idt_init();
	syscall_init(boot_info->stack_addr);
	keyboard_init();

	process_init(&(boot_info->module_info));

	// switch to userspace and start osl shell program

	while(1) {
		asm("hlt");
	}
}
