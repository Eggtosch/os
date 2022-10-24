#pragma once

#include <common.h>


enum MEM_TYPE {
	MEM_USABLE                 = 0x0001,
	MEM_RESERVED               = 0x0002,
	MEM_ACPI_RECLAIMABLE       = 0x0003,
	MEM_ACPI_NVS               = 0x0004,
	MEM_BAD_MEMORY             = 0x0005,
	MEM_BOOTLOADER_RECLAIMABLE = 0x1000,
	MEM_KERNEL_AND_MODULES     = 0x1001,
	MEM_FRAMEBUFFER            = 0x1002
};

struct boot_info {

	void (*fb_print)(const char *msg, u64 len);
	void *stack_addr;
	void *rsdp;

	struct fb_info {
		u32 *fb_addr;
		u16 fb_width;
		u16 fb_height;
		u16 fb_pitch;
	} fb_info;

	struct mem_info {
		u64 mem_entries;

		struct mem_entry {
			void *mem_base;
			u64 mem_length;
			u32 mem_type;
			u32 mem_unused;
		} *mem_map;

		u64 mem_pmm_base;
		u64 mem_vmm_base;

	} mem_info;

	struct module_info {
		u64 module_count;

		struct kernel_module {
			void *begin;
			void *end;
			char string[128];
		} *modules;

	} module_info;

};

struct boot_info *boot_info_get(void);

