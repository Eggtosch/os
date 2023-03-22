#pragma once

#include <common.h>

#define ELF_OK (0)
#define ELF_WRONG_HEADER (1)
#define ELF_NO_EXEC (2)
#define ELF_WRONG_BITS (3)
#define ELF_WRONG_ARCH (4)

int elf_validate(u8 *elf);
int elf_load(u8 *elf, u64 **pagedir, u64 *entry);
const char *elf_error_str(int err);
