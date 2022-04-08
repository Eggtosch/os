#include <boot/boot_info.h>

#include <device/serial.h>
#include <device/framebuffer.h>
#include <console/console.h>
#include <io/stdio.h>
#include <debug.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <vfs/vfs.h>

#include <interrupts/idt.h>
#include <syscall/syscall.h>
#include <device/keyboard.h>

#include <process/process.h>


void kmain(struct boot_info *boot_info) {
	serial_init();
	u64 fb_ret = framebuffer_init(&(boot_info->fb_info));
	console_init();
	stdio_init(console_io);
	debug(DEBUG_INFO, "Initialized serial");
	debug(DEBUG_INFO, "Initialized framebuffer (write combining %ssupported)",
						fb_ret != 0 ? "un" : "");
	debug(DEBUG_INFO, "Initialized console");
	debug(DEBUG_INFO, "Initialized stdio system");

	pmm_init(&(boot_info->mem_info));
	vmm_init();

	console_init_buffering(console_io);

	vfs_init();

	serial_init_vfs();

	idt_init();
	syscall_init(boot_info->stack_addr);
	keyboard_init();

	process_init(&(boot_info->module_info));
	process_create("shell.osl");

	while(1) {
		asm("hlt");
	}
}
