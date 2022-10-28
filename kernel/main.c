#include <boot/boot_info.h>

#include <io/stdio.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <acpi/acpi.h>

#include <vfs/vfs.h>

#include <interrupts/idt.h>
#include <syscall/syscall.h>

#include <driver/driver.h>
#include <driver/util.h>

extern driver_init_t __start_driver_init[];
extern driver_init_t __stop_driver_init[];

void kmain(struct boot_info *boot_info) {
	stdio_init(serial_io_device_get());

	pmm_init(boot_info);
	vmm_init();

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

	printf("free bytes: %d\n", pmm_get_free_bytes());

	i64 t = time();
	i64 time_printed = 0;

	while(1) {
		asm("hlt");

		if (time() != t) {
			t = time();
			continue;
		}

		if (t == 0 || t == time_printed) {
			continue;
		}
		time_printed = t;

		char datebuf[100];
		u64 len = vfs_read("/dev/date", (u8*) datebuf, 100);

		boot_info->fb_print(datebuf, len);
		boot_info->fb_print("\n", 1);
	}
}

