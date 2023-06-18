#include <memory/vmm.h>

#include <memory/pmm.h>

#include <string.h>
#include <panic.h>
#include <common.h>

static pagedir_t *_kernel_page_dir;
static u64 _higher_half_map_index;
static u64 _kernel_map_index;

static void save_kernel_pagedir_entries(void) {
	u64 hhdm_start = 0xffff800000000000UL;
	u64 kmap_start = 0xffffffff80000000UL;
	_higher_half_map_index = (hhdm_start >> 39) & 0x1ff;
	_kernel_map_index = (kmap_start >> 39) & 0x1ff;
}

static pagedir_t *next_pd_level(pagedir_t *pagedirx, u64 index) {
	return (pagedir_t*)(pagedirx[index] & ~((u64) 0xfff));
}

static u64 entry_to_address(pagedir_t *pagedir_entry) {
	return (u64) pagedir_entry & ~((u64) 0xfff);
}

static u64 get_num_of_pages(u64 bytes) {
	return bytes / PAGE_SIZE + (bytes % PAGE_SIZE == 0 ? 0 : 1);
}

static void free_entry(pagedir_t *pagedirx) {
	void *virt_addr = (void*) entry_to_address(pagedirx);
	pmm_free(virt_addr, 1);
}

static void destroy_pd1(pagedir_t *pd1) {
	pd1 = pmm_to_virt((u64) pd1);
	for (u64 i1 = 0; i1 < 512; i1++) {
		if (pd1[i1] & 1) {
			free_entry((pagedir_t*) pd1[i1]);
		}
	}
	free_entry(pd1);
}

static void destroy_pd2(pagedir_t *pd2) {
	pd2 = pmm_to_virt((u64) pd2);
	for (u64 i2 = 0; i2 < 512; i2++) {
		if (pd2[i2] & 1) {
			destroy_pd1(next_pd_level(pd2, i2));
			pd2[i2] = 0;
		}
	}
	free_entry(pd2);
}

static void destroy_pd3(pagedir_t *pd3) {
	pd3 = pmm_to_virt((u64) pd3);
	for (u64 i3 = 0; i3 < 512; i3++) {
		if (pd3[i3] & 1) {
			destroy_pd2(next_pd_level(pd3, i3));
		}
	}
	free_entry(pd3);
}

static void destroy_pd4(pagedir_t *pd4) {
	pd4 = pmm_to_virt((u64) pd4);
	for (u64 i4 = 0; i4 < 512; i4++) {
		if (i4 == _kernel_map_index ||
			i4 == _higher_half_map_index) {
			continue;
		}
		if (pd4[i4] & 1) {
			destroy_pd3(next_pd_level(pd4, i4));
		}
	}
	free_entry(pd4);
}

static pagedir_t *get_pagedir_level(pagedir_t *pagedirx, u64 index) {
	pagedirx = pmm_to_virt((u64) pagedirx);
	if (!(pagedirx[index] & 1)) {
		pagedirx[index] = pmm_to_phys(pmm_alloc(1)) | 0b111;
	}
	return pmm_to_virt(pagedirx[index] & ~511);
}

static u64 *pd_entry(pagedir_t *pagedir, void *virt_addr) {
	u64 addr_val = (u64) virt_addr;

	u64 index4 = (addr_val & ((u64)0x1ff << 39)) >> 39;
	u64 index3 = (addr_val & ((u64)0x1ff << 30)) >> 30;
	u64 index2 = (addr_val & ((u64)0x1ff << 21)) >> 21;
	u64 index1 = (addr_val & ((u64)0x1ff << 12)) >> 12;

	pagedir_t *pagedir4 = pagedir;
	pagedir_t *pagedir3 = get_pagedir_level(pagedir4, index4);
	pagedir_t *pagedir2 = get_pagedir_level(pagedir3, index3);
	pagedir_t *pagedir1 = get_pagedir_level(pagedir2, index2);

	return &pagedir1[index1];
}

static void vmm_map_page(pagedir_t *pagedir, u64 phys_addr, void *virt_addr) {
	u64 *entry = pd_entry(pagedir, virt_addr);
	*entry = phys_addr | 0b111;
	asm volatile("invlpg (%0)" :: "r"(virt_addr));
}

static u64 vmm_unmap_page(pagedir_t *pagedir, void *virt_addr) {
	u64 *entry = pd_entry(pagedir, virt_addr);
	u64 phys_addr = entry_to_address((pagedir_t*) entry);
	*entry = 0;
	return phys_addr;
}

void vmm_init(void) {
	_kernel_page_dir = vmm_get_pagedir();
	save_kernel_pagedir_entries();
	kprintf("kernel page directory saved\n");
}

pagedir_t *vmm_get_pagedir(void) {
	u64 pd;
	asm volatile("mov %%cr3, %0" : "=r"(pd) :: "memory");
	return pmm_to_virt(pd);
}

void vmm_set_pagedir(pagedir_t *pd) {
	if (pd == NULL) {
		pd = _kernel_page_dir;
	}

	pd = (pagedir_t*) pmm_to_phys(pd);
	asm volatile("mov %0, %%cr3" :: "r"(pd) : "memory");
}

bool vmm_kernel_pagedir_active(void) {
	return vmm_get_pagedir() == _kernel_page_dir;
}

pagedir_t *vmm_pagedir_create(void) {
	pagedir_t *pagedir = pmm_alloc(1);
	pagedir[_higher_half_map_index] = _kernel_page_dir[_higher_half_map_index];
	pagedir[_kernel_map_index]      = _kernel_page_dir[_kernel_map_index];
	return pagedir;
}

void vmm_pagedir_destroy(pagedir_t *pagedir) {
	if (pagedir == NULL) {
		return;
	}

	if (pagedir == _kernel_page_dir) {
		panic("Trying to destroy the kernel page dir!");
	}

	destroy_pd4(pagedir);
}

void vmm_map(pagedir_t *pagedir, void *virt_addr, u64 size) {
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

void vmm_map_data(pagedir_t *pagedir, void *virt_addr, void *data, u64 data_size) {
	if (pagedir == NULL) {
		pagedir = vmm_get_pagedir();
	}

	u64 pages = get_num_of_pages(data_size);
	u64 alloced = pmm_to_phys(pmm_alloc(pages));
	for (u64 i = 0; i < pages; i++) {
		u64 page = alloced + i * PAGE_SIZE;
		vmm_map_page(pagedir, page, virt_addr + PAGE_SIZE * i);
		u64 bytes_to_copy = data_size >= PAGE_SIZE ? PAGE_SIZE : data_size;
		memcpy(pmm_to_virt(page), data, bytes_to_copy);
		data_size -= bytes_to_copy;
		data += bytes_to_copy;
	}
}

u64 vmm_vaddr_to_phys(pagedir_t *pagedir, void *virt_addr) {
	u64 page_offset = (u64) virt_addr & 0xfff;
	u64 *entryp = pd_entry(pagedir, virt_addr);
	u64 entry = *entryp & 0x7fffffffff000;
	return entry + page_offset;
}

void vmm_unmap(pagedir_t *pagedir, void *virt_addr, u64 size) {
	if (pagedir == NULL) {
		pagedir = vmm_get_pagedir();
	}

	u64 pages = get_num_of_pages(size);
	for (u64 i = 0; i < pages; i++) {
		u64 phys_addr = vmm_unmap_page(pagedir, virt_addr);
		pmm_free(pmm_to_virt(phys_addr), 1);
	}
}
