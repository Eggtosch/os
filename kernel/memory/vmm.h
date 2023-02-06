#pragma once

#include <common.h>

#define VMM_USER_CODE      (0x400000)
#define VMM_USER_STACK_END (0x800000000000)
#define VMM_USER_STACK_LEN (0x10000)

typedef u64 pagedir_t;

void vmm_init(void);
pagedir_t *vmm_get_pagedir(void);
void vmm_set_pagedir(pagedir_t *pagedir);

bool vmm_kernel_pagedir_active(void);

pagedir_t *vmm_pagedir_create(void);
void vmm_pagedir_destroy(pagedir_t *pagedir);

void vmm_map(pagedir_t *pagedir, void *virt_addr, u64 size);
void vmm_map_data(pagedir_t *pagedir, void *virt_addr, void *data, u64 data_size);
u64 vmm_vaddr_to_phys(pagedir_t *pagedir, void *virt_addr);
void vmm_unmap(pagedir_t *pagedir, void *virt_addr, u64 size);

