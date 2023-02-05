#include <memory/vmm.h>

#include <memory/pmm.h>

#include <string.h>
#include <panic.h>
#include <common.h>


static u64 *_kernel_page_dir;
static u64 _higher_half_map_index;
static u64 _kernel_map_index;

static void save_kernel_pagedir_entries(void) {
	u64 hhdm_start = 0xffff800000000000UL;
	u64 kmap_start = 0xffffffff80000000UL;
	_higher_half_map_index = (hhdm_start >> 39) & 0x1ff;
	_kernel_map_index = (kmap_start >> 39) & 0x1ff;
}

static u64 *next_pd_level(u64 *pagedirx, u64 index) {
	return (u64*)(pagedirx[index] & ~((u64) 0x1ff));
}

static u64 entry_to_address(u64 *pagedir_addr) {
	return (u64) pagedir_addr & ~((u64) 0x1ff);
}

static u64 get_num_of_pages(u64 bytes) {
	return bytes / PAGE_SIZE + (bytes % PAGE_SIZE == 0 ? 0 : 1);
}

static void destroy_pd1(u64 *pd1) {
	pd1 = (u64*) pmm_to_virt((u64) pd1);
	for (u64 i1 = 0; i1 < 512; i1++) {
		if (pd1[i1] & 1) {
			u64 addr = entry_to_address((u64*) pd1[i1]);
			pmm_free(addr, 1);
		}
	}
	pmm_free(entry_to_address(pd1), 1);
}

static void destroy_pd2(u64 *pd2) {
	pd2 = (u64*) pmm_to_virt((u64) pd2);
	for (u64 i2 = 0; i2 < 512; i2++) {
		if (pd2[i2] & 1) {
			destroy_pd1(next_pd_level(pd2, i2));
			pd2[i2] = 0;
		}
	}
	pmm_free(entry_to_address(pd2), 1);
}

static void destroy_pd3(u64 *pd3) {
	pd3 = (u64*) pmm_to_virt((u64) pd3);
	for (u64 i3 = 0; i3 < 512; i3++) {
		if (pd3[i3] & 1) {
			destroy_pd2(next_pd_level(pd3, i3));
		}
	}
	pmm_free(entry_to_address(pd3), 1);
}

static void destroy_pd4(u64 *pd4) {
	pd4 = (u64*) pmm_to_virt((u64) pd4);
	for (u64 i4 = 0; i4 < 512; i4++) {
		if (i4 == _kernel_map_index ||
			i4 == _higher_half_map_index) {
			continue;
		}
		if (pd4[i4] & 1) {
			destroy_pd3(next_pd_level(pd4, i4));
		}
	}
	pmm_free(entry_to_address(pd4), 1);
}

static u64 *get_pagedir_level(u64 *pagedirx, u64 index) {
	pagedirx = (u64*) pmm_to_virt((u64) pagedirx);
	if (!(pagedirx[index] & 1)) {
		pagedirx[index] = pmm_to_phys(pmm_alloc(1)) | 0b111;
	}
	return (u64*) pmm_to_virt(pagedirx[index] & ~511);
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
	asm volatile("invlpg (%0)" :: "r"(virt_addr));
}

static u64 *vmm_unmap_page(u64 *pagedir, u64 virt_addr) {
	u64 index4 = (virt_addr & ((u64)0x1ff << 39)) >> 39;
	u64 index3 = (virt_addr & ((u64)0x1ff << 30)) >> 30;
	u64 index2 = (virt_addr & ((u64)0x1ff << 21)) >> 21;
	u64 index1 = (virt_addr & ((u64)0x1ff << 12)) >> 12;

	u64 *pagedir4 = pagedir;
	u64 *pagedir3 = get_pagedir_level(pagedir4, index4);
	u64 *pagedir2 = get_pagedir_level(pagedir3, index3);
	u64 *pagedir1 = get_pagedir_level(pagedir2, index2);

	u64 phys_addr = entry_to_address((u64*) pagedir1[index1]);
	pagedir1[index1] = 0;
	return (u64*) phys_addr;
}

void vmm_init(void) {
	_kernel_page_dir = vmm_get_pagedir();
	save_kernel_pagedir_entries();
	kprintf("kernel page directory saved\n");
}

u64 *vmm_get_pagedir(void) {
	u64 pd;
	asm volatile("mov %%cr3, %0" : "=r"(pd) :: "memory");
	return (u64*) pmm_to_virt(pd);
}

void vmm_set_pagedir(u64 *pd) {
	if (pd == NULL) {
		pd = _kernel_page_dir;
	}
	asm volatile("mov %0, %%cr3" :: "r"(pmm_to_phys((u64) pd)) : "memory");
}

bool vmm_kernel_pagedir_active(void) {
	return vmm_get_pagedir() == _kernel_page_dir;
}

u64 *vmm_pagedir_create(void) {
	u64 *pagedir = (u64*) pmm_alloc(1);
	pagedir[_higher_half_map_index] = _kernel_page_dir[_higher_half_map_index];
	pagedir[_kernel_map_index]      = _kernel_page_dir[_kernel_map_index];
	return pagedir;
}

void vmm_pagedir_destroy(u64 *pagedir) {
	if (pagedir == NULL) {
		return;
	}

	if (pagedir == _kernel_page_dir) {
		panic("Trying to destroy the kernel page dir!");
	}

	destroy_pd4(pagedir);
}

void vmm_map(u64 *pagedir, u64 virt_addr, u64 size) {
	if (pagedir == NULL) {
		pagedir = vmm_get_pagedir();
	}
	u64 pages = get_num_of_pages(size);
	u64 alloced = pmm_to_phys(pmm_alloc(pages));
	for (u64 i = 0; i < pages; i++) {
		u64 page = alloced + i * PAGE_SIZE;
		vmm_map_page(pagedir, page, virt_addr + PAGE_SIZE * i);
	}
}

void vmm_map_data(u64 *pagedir, u64 virt_addr, void *data, u64 data_size) {
	if (pagedir == NULL) {
		pagedir = vmm_get_pagedir();
	}
	u64 pages = get_num_of_pages(data_size);
	u64 alloced = pmm_to_phys(pmm_alloc(pages));
	for (u64 i = 0; i < pages; i++) {
		u64 page = alloced + i * PAGE_SIZE;
		vmm_map_page(pagedir, page, virt_addr + PAGE_SIZE * i);
		u64 bytes_to_copy = data_size >= PAGE_SIZE ? PAGE_SIZE : data_size;
		memcpy((void*) page, data, bytes_to_copy);
		data_size -= bytes_to_copy;
		data += bytes_to_copy;
	}
}

u64 vmm_vaddr_to_phys(u64 *pagedir, u64 virt_addr) {
	u64 page_offset = virt_addr & 0xfff;
	virt_addr = virt_addr & ~((u64) 0xfff);
	u64 index4 = (virt_addr & ((u64)0x1ff << 39)) >> 39;
	u64 index3 = (virt_addr & ((u64)0x1ff << 30)) >> 30;
	u64 index2 = (virt_addr & ((u64)0x1ff << 21)) >> 21;
	u64 index1 = (virt_addr & ((u64)0x1ff << 12)) >> 12;

	u64 *pagedir4 = pagedir;
	u64 *pagedir3 = get_pagedir_level(pagedir4, index4);
	u64 *pagedir2 = get_pagedir_level(pagedir3, index3);
	u64 *pagedir1 = get_pagedir_level(pagedir2, index2);

	return (pagedir1[index1] & ~((u64) 0b111)) + page_offset;
}

void vmm_unmap(u64 *pagedir, u64 virt_addr, u64 size) {
	if (pagedir == NULL) {
		pagedir = vmm_get_pagedir();
	}
	u64 pages = get_num_of_pages(size);
	for (u64 i = 0; i < pages; i++) {
		u64 *phys_addr = vmm_unmap_page(pagedir, virt_addr);
		pmm_free((u64) phys_addr, 1);
	}
}

