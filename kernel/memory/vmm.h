#pragma once

#include <common.h>


#define VMM_USER_CODE      (0x400000)
#define VMM_USER_STACK_END (0x800000000000)
#define VMM_USER_STACK_LEN (0x10000)

void vmm_init(void);
u64 *vmm_get_pagedir(void);
void vmm_set_pagedir(u64 *pagedir);

u64 *vmm_pagedir_create(void);
void vmm_pagedir_destroy(u64 *pagedir);

void vmm_map(u64 *pagedir, u64 virt_addr, u64 size);
void vmm_map_data(u64 *pagedir, u64 virt_addr, void *data, u64 data_size);
u64 vmm_vaddr_to_phys(u64 *pagedir, u64 virt_addr);
void vmm_unmap(u64 *pagedir, u64 virt_addr, u64 size);

