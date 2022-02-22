#pragma once

#include <boot/boot_info.h>
#include <common.h>

#define PAGE_SIZE (4096)

#define VIRTUAL_ADDR_OFFSET ((uintptr_t) 0xffff800000000000)

#define VIRTUAL_TO_PHYSICAL(vaddr) ( ((uintptr_t) vaddr) - (VIRTUAL_ADDR_OFFSET) )
#define PHYSICAL_TO_VIRTUAL(paddr) ( ((uintptr_t) paddr) + (VIRTUAL_ADDR_OFFSET) )

#if 0
static uintptr_t physical_to_kerneladdr(uintptr_t physical_addr) {
	return physical_addr + (_mem_info->mem_vmm_base - _mem_info->mem_pmm_base);
}

static uintptr_t kerneladdr_to_physical(uintptr_t virtual_addr) {
	return virtual_addr - (_mem_info->mem_vmm_base + _mem_info->mem_pmm_base);
}
#endif

void pmm_init(struct mem_info *mem_info);
void *pmm_alloc(size_t page_count);
void pmm_free(void *ptr, size_t page_count);
