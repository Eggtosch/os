#pragma once

void vmm_init(void);
u64 *vmm_get_pagedir(void);
void vmm_set_pagedir(u64 *pagedir);

u64 *vmm_pagedir_create(void);
void vmm_pagedir_destroy(u64 *pagedir);

// maybe extend pmm bitmap for physical memory by attributes like alloced (yes:no)
// vmm_map for allocating phys page and vmm_map for mapping existing page?
void vmm_map(u64 *pagedir, u64 virt_addr, u64 size);
void vmm_map_data(u64 *pagedir, u64 virt_addr, void *data, u64 data_size);
//void vmm_unmap(u64 *pagedir, u64 virt_addr, u64 page_count);
