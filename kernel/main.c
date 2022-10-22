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
u64 time(void);
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
		init->func(boot_info);
	}

	vfs_print_entries();

	printf("free bytes: %d\n", pmm_get_free_bytes());

	struct io_device *dev = vfs_get(vfs_open("/dev/date"));
	u64 t = time();

	while(1) {
		asm("hlt");

		if (time() == t) {
			continue;
		}
		t = time();

		char datebuf[100];
		u64 len = dev->read(dev, (u8*) datebuf, 100);

		printf("%s\n", datebuf);
		boot_info->early_boot_print(datebuf, len);
		boot_info->early_boot_print("\n", 1);
	}
}

