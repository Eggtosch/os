#include <common.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <debug.h>
#include <kexit.h>


static u64 *_kernel_page_dir;
static u64 _identity_map_index;
static u64 _higher_half_map_index;
static u64 _kernel_map_index;

static void save_kernel_pagedir_entries(void) {
	_identity_map_index    = 512;
	_higher_half_map_index = 512;
	_kernel_map_index      = 512;
	u64 i = 0;
	for (; i < 512; i++) {
		if (_kernel_page_dir[i] & 1) {
			_identity_map_index = i;
			i++;
			break;
		}
	}
	for (; i < 512; i++) {
		if (_kernel_page_dir[i] & 1) {
			_higher_half_map_index = i;
			i++;
			break;
		}
	}
	for (; i < 512; i++) {
		if (_kernel_page_dir[i] & 1) {
			_kernel_map_index = i;
			i++;
			break;
		}
	}
	if (_identity_map_index == 512 ||
		_higher_half_map_index == 512 ||
		_kernel_map_index == 512) {
		debug(DEBUG_INFO, "Could not find relevant entries in kernel page directory!");
		kexit();
	}
}

void vmm_init(void) {
	_kernel_page_dir = (u64*) vmm_get_pagedir();
	save_kernel_pagedir_entries();
	debug(DEBUG_INFO, "Initialized VMM: Kernel Page Directory at %p", _kernel_page_dir);
}

u64 *vmm_get_pagedir(void) {
	u64 *pd;
	asm volatile("mov %%cr3, %0" : "=r"(pd) :: "memory");
	return (u64*) PHYSICAL_TO_VIRTUAL(pd);
}

void vmm_set_pagedir(u64 *pd) {
	asm volatile("mov %0, %%cr3" :: "r"(VIRTUAL_TO_PHYSICAL(pd)) : "memory");
}

u64 *vmm_pagedir_create(void) {
	u64 *pagedir = pmm_alloc(1);
	pagedir[_identity_map_index]    = _kernel_page_dir[_identity_map_index];
	pagedir[_higher_half_map_index] = _kernel_page_dir[_higher_half_map_index];
	pagedir[_kernel_map_index]      = _kernel_page_dir[_kernel_map_index];
	return pagedir;
}

void vmm_pagedir_destroy(u64 *pagedir) {
	pmm_free(pagedir, 1);
}

static u64 *get_pagedir_level(u64 *pagedirx, u64 index) {
	if (!(pagedirx[index] & 1)) {
		pagedirx[index] = VIRTUAL_TO_PHYSICAL(pmm_alloc(1)) | 0b111;
	}
	return (u64*) (pagedirx[index] & ~511);
}

static void vmm_map_page(u64 *pagedir, u64 phys_addr, u64 virt_addr) {
	u64 index4 = (virt_addr & ((u64)0x1ff << 39)) >> 39;
	u64 index3 = (virt_addr & ((u64)0x1ff << 30)) >> 30;
	u64 index2 = (virt_addr & ((u64)0x1ff << 21)) >> 21;
	u64 index1 = (virt_addr & ((u64)0x1ff << 12)) >> 12;

	u64 *pagedir4 = pagedir;
	u64 *pagedir3 = get_pagedir_level(pagedir4, index4);
	u64 *pagedir2 = get_pagedir_level(pagedir3, index3);
	u64 *pagedir1 = get_pagedir_level(pagedir2, index2);

	pagedir1[index1] = phys_addr | 0b111;
	asm volatile("invlpg (%0)" : : "r" (virt_addr));
}

void vmm_map(u64 *pagedir, u64 virt_addr, u64 size) {
	u64 pages = size / PAGE_SIZE + 1;
	for (u64 i = 0; i < pages; i++) {
		u64 page = VIRTUAL_TO_PHYSICAL(pmm_alloc(1));
		vmm_map_page(pagedir, page, virt_addr + PAGE_SIZE * i);
	}
}

void vmm_map_data(u64 *pagedir, u64 virt_addr, void *data, u64 data_size) {
	u64 pages = data_size / PAGE_SIZE + 1;
	for (u64 i = 0; i < pages; i++) {
		u64 page = VIRTUAL_TO_PHYSICAL(pmm_alloc(1));
		vmm_map_page(pagedir, page, virt_addr + PAGE_SIZE * i);
		for (u64 n = 0; n < PAGE_SIZE && data_size > 0; n++, data_size--) {
			((u8*) page)[n] = ((u8*) data)[n];
		}
	}
}
