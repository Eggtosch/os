#include <common.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <debug.h>

typedef u64 *page_dir_t;


static page_dir_t _kernel_page_dir;

static page_dir_t get_active_pagedir(void) {
	page_dir_t pd;
	asm volatile("mov %%cr3, %0" : "=r"(pd) :: "memory");
	return pd;
}

/*static void set_active_pagedir(page_dir_t pd) {
	asm volatile("mov %0, %%cr3" :: "r"(VIRTUAL_TO_PHYSICAL(pd)) : "memory");
}*/


void vmm_init(void) {
	_kernel_page_dir = (page_dir_t) PHYSICAL_TO_VIRTUAL(get_active_pagedir());
	debug(DEBUG_INFO, "Initialized VMM: Kernel Page Directory at %p", _kernel_page_dir);
}
