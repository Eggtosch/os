#include <boot/boot_info.h>

#include <io/stdio.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <vfs/vfs.h>

#include <interrupts/idt.h>
#include <syscall/syscall.h>

#include <driver/driver.h>
#include <process/process.h>

struct io_device *serial_get_io_device(void);
extern driver_init_t __start_driver_init[];
extern driver_init_t __stop_driver_init[];

void kmain(struct boot_info *boot_info) {
	stdio_init(serial_get_io_device());

	pmm_init(&(boot_info->mem_info));
	vmm_init();

	vfs_init();

	idt_init();
	syscall_init(boot_info->stack_addr);

	u64 ndrivers = (__stop_driver_init - __start_driver_init);
	for (u64 i = 0; i < ndrivers; i++) {
		driver_init_t *init = __start_driver_init + i;
		printf("Init driver %s\n", init->name);
		init->func(boot_info);
	}

	vfs_print_entries();

	printf("free bytes: %d\n", pmm_get_free_bytes());

	//process_init(&(boot_info->module_info));
	//process_create("shell.osl");

	while(1) {
		asm("hlt");
	}
}

