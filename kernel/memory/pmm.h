#pragma once

#include <boot/boot_info.h>
#include <common.h>

void pmm_init(struct boot_info *boot_info);
void *pmm_alloc(u64 page_count);
void pmm_free(void *ptr, u64 page_count);
u64  pmm_get_free_bytes(void);
u64  pmm_to_phys(void* vaddr);
void* pmm_to_virt(u64 paddr);

