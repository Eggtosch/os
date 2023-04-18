#pragma once

#include <common.h>
#include <boot/boot_info.h>

#define ELF_OK (0)
#define ELF_WRONG_HEADER (1)
#define ELF_NO_EXEC (2)
#define ELF_WRONG_BITS (3)
#define ELF_WRONG_ARCH (4)

void elf_set_kernel_info(struct boot_info *boot_info);
const char *elf_get_kernel_symbol(void *addr);

int elf_validate(u8 *elf);
int elf_load(u8 *elf, u64 **pagedir, u64 *entry);
const char *elf_error_str(int err);
