#pragma once

#include <boot/boot_info.h>
#include <common.h>


#define VIRTUAL_ADDR_OFFSET ((u64) 0xffff800000000000)

#define VIRTUAL_TO_PHYSICAL(vaddr) (((u64) vaddr) - (VIRTUAL_ADDR_OFFSET))
#define PHYSICAL_TO_VIRTUAL(paddr) (((u64) paddr) + (VIRTUAL_ADDR_OFFSET))

void pmm_init(struct mem_info *mem_info);
u64  pmm_alloc(u64 page_count);
void pmm_free(u64 ptr, u64 page_count);
