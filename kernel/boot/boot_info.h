#pragma once

#include <common.h>


enum MEM_TYPE {
	MEM_USABLE                 = 0,
	MEM_RESERVED               = 1,
	MEM_ACPI_RECLAIMABLE       = 2,
	MEM_ACPI_NVS               = 3,
	MEM_BAD_MEMORY             = 4,
	MEM_BOOTLOADER_RECLAIMABLE = 5,
	MEM_KERNEL_AND_MODULES     = 6,
	MEM_FRAMEBUFFER            = 7
};

enum MEDIA_TYPE {
	MEDIA_GENERIC = 0,
	MEDIA_OPTICAL = 1,
	MEDIA_TFTP    = 2
};

struct uuid {
	u32 a;
	u16 b;
	u16 c;
	u8 d[8];
};

struct boot_info {

	void (*fb_print)(const char *msg, u64 len);
	void *stack_addr;
	void *rsdp;
	void *efi_ptr;

	struct fb_info {
		u32 *fb_addr;
		u16 fb_width;
		u16 fb_height;
		u16 fb_pitch;
		u64 fb_nmodes;
		struct fb_mode {
			u64 fb_pitch;
			u64 fb_width;
			u64 fb_height;
			u16 fb_bpp;
		} **fb_modes;
	} fb_info;

	struct mem_info {
		u64 mem_entries;

		struct mem_entry {
			void *mem_base;
			u64 mem_length;
			u64 mem_type;
		} **mem_map;

		u64 mem_pmm_base;
		u64 mem_vmm_base;

	} mem_info;

	struct module_info {
		u64 module_count;

		struct kernel_module {
			u64 revision;
			void *addr;
			u64 size;
			char *path;
			char *cmdline;
			u32 media_type;
			u32 unused;
			u32 tftp_ip;
			u32 tftp_port;
			u32 partition_index;
			u32 mbr_disk_id;
			struct uuid gpt_disk_uuid;
			struct uuid gpt_part_uuid;
			struct uuid part_uuid;
		} **modules;

	} module_info;

};

struct boot_info *boot_info_get(void);

