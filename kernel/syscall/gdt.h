#pragma once

#define SEGMENT_KERNEL_CODE (0x28)
#define SEGMENT_USER_CODE (0x38)

void gdt_init(void *kernel_stack);
