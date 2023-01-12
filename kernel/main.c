#include <boot/boot_info.h>

#include <io/stdio.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <cpu/cpu.h>
#include <acpi/acpi.h>

#include <vfs/vfs.h>

#include <interrupts/idt.h>
#include <syscall/syscall.h>

#include <driver/driver.h>
#include <driver/util.h>

#include <process/process.h>

extern driver_init_t __start_driver_init[];
extern driver_init_t __stop_driver_init[];

void kmain(struct boot_info *boot_info) {
	stdio_init(serial_io_device_get());

	pmm_init(boot_info);
	vmm_init();

	cpu_init(boot_info);

	acpi_init(boot_info);

	vfs_init();

	idt_init();
	syscall_init(boot_info);

	u64 ndrivers = (__stop_driver_init - __start_driver_init);
	for (u64 i = 0; i < ndrivers; i++) {
		driver_init_t *init = __start_driver_init + i;
		init->func(boot_info);
	}

	vfs_print_entries();

	process_init(boot_info);
	process_create("/modules/init.elf");

	kloop();
}

void kloop(void) {
	struct boot_info *boot_info = boot_info_get();
	char buf[80];
	int fd = vfs_open("/dev/date");
	struct io_device *dev = vfs_get(fd);
	while(1) {
		asm("sti");
		asm("hlt");
		int len = dev->read(dev, (u8*) buf, 80, 0);
		boot_info->fb_print(buf, len);
		boot_info->fb_print("\r", 1);
		printf("%s\r", buf);
	}
}

