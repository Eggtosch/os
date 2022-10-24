#pragma once

#include <boot/boot_info.h>
#include <common.h>


#define VIRTUAL_ADDR_OFFSET ((u64) 0xffff800000000000)

void pmm_init(struct boot_info *boot_info);
u64  pmm_alloc(u64 page_count);
void pmm_free(u64 ptr, u64 page_count);
u64  pmm_get_free_bytes(void);
u64  pmm_to_phys(u64 vaddr);
u64  pmm_to_virt(u64 paddr);

