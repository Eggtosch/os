#include <boot/boot_info.h>

#include <io/stdio.h>
#include <mutex.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <framebuffer/framebuffer.h>

#include <acpi/acpi.h>
#include <cpu/cpu.h>
#include <interrupts/lapic.h>

#include <vfs/vfs.h>

#include <interrupts/idt.h>
#include <syscall/efi.h>
#include <syscall/syscall.h>
#include <time/hpet.h>
#include <time/time.h>

#include <driver/driver.h>
#include <driver/util.h>

#include <process/elf.h>
#include <process/kernel_task.h>
#include <process/process.h>
#include <process/scheduler.h>

#include <panic.h>

static void f(__unused struct smp_cpu *cpu) {
	apic_init(cpu->lapic_id);
	idt_init();
	syscall_init(boot_info_get());
	scheduler_start();

	kloop();
}

static void ktask0(void) {
	while (1) {
		kprintf("[%d] ktask 0\n", apic_current_cpu());
		asm("hlt");
	}
}

static void ktask1(void) {
	while (1) {
		kprintf("[%d] ktask 1\n", apic_current_cpu());
		asm("hlt");
	}
}

extern driver_init_t __start_driver_init[];
extern driver_init_t __stop_driver_init[];

static mutex_t kprintf_lock;

void kmain(struct boot_info *boot_info) {
	mutex_init(&kprintf_lock);

	efi_functions_init(boot_info);
	stdio_init(serial_io_device_get());
	kprintf("collected boot information\n");
	kprintf("booted with %s\n", efi_supported() ? "uefi" : "bios");
	kprintf("initialized early stdio output (serial device)\n");

	elf_set_kernel_info(boot_info);

	pmm_init(boot_info);
	vmm_init();

	framebuffer_init(boot_info);

	cpu_init(boot_info);
	apic_check_features();
	acpi_init(boot_info);

	vfs_init();

	idt_init();
	time_init();
	syscall_init(boot_info);

	kernel_task_system_init();

	for (u32 i = 0; i < boot_info->smp_info.cpu_count; i++) {
		boot_info->smp_info.smp_cpus[i]->cpu_entry = f;
	}

	u64 ndrivers = (__stop_driver_init - __start_driver_init);
	kprintf("found %d available drivers\n", ndrivers);
	for (u64 i = 0; i < ndrivers; i++) {
		driver_init_t *init = __start_driver_init + i;
		kprintf("loading driver: %s\n", init->name);
		init->func(boot_info);
	}

	process_init(boot_info);
	process_start_init("/modules/init");
	scheduler_init();

	tid_t k0 = kernel_task_create("ktask0", ktask0);
	tid_t k1 = kernel_task_create("ktask1", ktask1);
	scheduler_start();

	kloop();
}

__attribute__((noreturn)) void kloop(void) {
	if (!vmm_kernel_pagedir_active()) {
		vmm_set_pagedir(NULL);
		panic("kloop executed when other process was running!");
	}

	while (1) {
		asm("sti");
		asm("hlt");
	}
}

void kprintf(const char *fmt, ...) {
	mutex_lock(&kprintf_lock);

	i64 secs = time_since_boot();
	u64 ns   = time_current_ns();

	struct boot_info *boot_info = boot_info_get();
	char buf[200];
	int len = snprintf(buf, sizeof(buf), "[%.5u.%0.6u] ", secs == -1 ? 0 : secs, ns / 1000);
	boot_info->fb_print(buf, len);

	va_list args;
	va_start(args, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	boot_info->fb_print(buf, len);
	va_end(args);

	mutex_unlock(&kprintf_lock);
}

static const char *error_strings[] = {
	[ENOENT] = "No such file or directory",
	[ESRCH]  = "No such process",
};

const char *strerror(int err) {
	if (err >= 0 && err < LAST_ERROR) {
		return error_strings[err];
	}

	if (err < 0 && err > -LAST_ERROR) {
		return error_strings[-err];
	}

	return "Unknown error code";
}
