#include <io/stdio.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <process/elf.h>
#include <string.h>

#define PT_LOAD (1)

const char *error_strings[] = {
	"ok",
	"data has invalid elf signature",
	"elf file is not an executable",
	"executable is not 64 bits",
	"executable is not x86-64",
};

const char *elf_error_str(int err) {
	if (err < 0 || err > (int) (sizeof(error_strings) / sizeof(error_strings[0]))) {
		return "unknown error";
	}
	return error_strings[err];
}

#define ELF_TYPE_EXECUTABLE (2)
#define ELF_BITS_64 (2)
#define ELF_ARCH_X86_64 (0x3e)

struct elf_header {
	u8 header[4];
	u8 bits;
	u8 byte_order;
	u8 elf_hdr_version;
	u8 os_abi;
	u8 reserved[8];
	u16 type;
	u16 arch;
	u32 elf_version;
	u64 entry;
	u64 phdr_offset;
	u64 shdr_offset;
	u32 flags;
	u16 hdr_size;
	u16 phdr_size;
	u16 phdr_count;
	u16 shdr_size;
	u16 shdr_count;
	u16 shstrndx;
};

struct elf64_phdr {
	u32 type;
	u32 flags;
	u64 offset;
	void *vaddr;
	u64 paddr;
	u64 filesize;
	u64 memsize;
	u64 align;
};

struct elf64_shdr {
	u32 sh_name;
	u32 sh_type;
	u64 sh_flags;
	u64 sh_addr;
	u64 sh_offset;
	u64 sh_size;
	u32 sh_link;
	u32 sh_info;
	u64 sh_addralign;
	u64 sh_entsize;
};

struct elf64_symbol {
	u32 sym_name;
	u8 sym_info;
	u8 sym_other;
	u16 sym_shndx;
	u64 sym_value;
	u64 sym_size;
};

static_assert(sizeof(struct elf64_symbol) == 24, "");

__unused static void print_sections(u8 *elf) {
	struct elf_header *header = (struct elf_header *) elf;
	struct elf64_shdr *shstrtab =
		(void *) elf + (header->shdr_offset + header->shstrndx * header->shdr_size);
	char *names = (void *) elf + shstrtab->sh_offset;

	for (int i = 0; i < header->shdr_count; i++) {
		struct elf64_shdr *shdr = (void *) elf + (header->shdr_offset + i * header->shdr_size);
		kprintf("%s\n", &names[shdr->sh_name]);
	}
}

static struct elf64_shdr *find_section(u8 *elf, const char *section) {
	struct elf_header *header = (struct elf_header *) elf;
	struct elf64_shdr *shstrtab =
		(void *) elf + (header->shdr_offset + header->shstrndx * header->shdr_size);
	char *names = (void *) elf + shstrtab->sh_offset;

	for (int i = 0; i < header->shdr_count; i++) {
		struct elf64_shdr *shdr = (void *) elf + (header->shdr_offset + i * header->shdr_size);
		if (strcmp(&names[shdr->sh_name], section) == 0) {
			return shdr;
		}
	}

	return NULL;
}

__unused static void print_segments(u8 *elf) {
	struct elf_header *header = (struct elf_header *) elf;
	for (int i = 0; i < header->phdr_count; i++) {
		struct elf64_phdr *phdr = (void *) elf + (header->phdr_offset + i * header->phdr_size);
		if (phdr->type != PT_LOAD) {
			continue;
		}
		kprintf("load: vaddr = %#x, filesize = %#x, memsize = %#x\n", phdr->vaddr, phdr->filesize,
		        phdr->memsize);
	}
}

int elf_validate(u8 *elf) {
	static u8 elf_header[]    = {0x7f, 'E', 'L', 'F'};
	struct elf_header *header = (struct elf_header *) elf;

	if (memcmp(header->header, elf_header, 4) != 0) {
		return ELF_WRONG_HEADER;
	}

	if (header->type != ELF_TYPE_EXECUTABLE) {
		return ELF_NO_EXEC;
	}

	if (header->bits != ELF_BITS_64) {
		return ELF_WRONG_BITS;
	}

	if (header->arch != ELF_ARCH_X86_64) {
		return ELF_WRONG_ARCH;
	}

	return ELF_OK;
}

int elf_load(u8 *elf, pagedir_t **pagedir, u64 *entry) {
	struct elf_header *header = (struct elf_header *) elf;

	void *lowest_vaddr  = (void *) 0xffffffffffffffffUL;
	void *highest_vaddr = (void *) 0UL;
	for (int i = 0; i < header->phdr_count; i++) {
		struct elf64_phdr *phdr = (void *) elf + (header->phdr_offset + i * header->phdr_size);
		if (phdr->type != PT_LOAD) {
			continue;
		}
		if (phdr->vaddr == 0) {
			continue;
		}
		if (phdr->vaddr < lowest_vaddr) {
			lowest_vaddr = phdr->vaddr;
		}
		if (phdr->vaddr + phdr->memsize > highest_vaddr) {
			highest_vaddr = phdr->vaddr + phdr->memsize;
		}
	}

	u64 size      = ALIGNUP_PAGE(highest_vaddr - lowest_vaddr);
	pagedir_t *pd = vmm_pagedir_create();
	vmm_map(pd, lowest_vaddr, size);

	for (int i = 0; i < header->phdr_count; i++) {
		struct elf64_phdr *phdr = (void *) elf + (header->phdr_offset + i * header->phdr_size);
		if (phdr->type != PT_LOAD) {
			continue;
		}
		if (phdr->vaddr == 0) {
			continue;
		}

		u8 *dest = pmm_to_virt(vmm_vaddr_to_phys(pd, phdr->vaddr));
		u8 *src  = (u8 *) elf + phdr->offset;
		u64 size = phdr->filesize;
		memcpy(dest, src, size);
		if (phdr->filesize < phdr->memsize) {
			memset(dest + phdr->filesize, 0, phdr->memsize - phdr->filesize);
		}
	}

	*pagedir = pd;
	*entry   = header->entry;

	return ELF_OK;
}

static u8 *kernel_elf                   = NULL;
static u64 kernel_elf_size              = 0;
static struct elf64_shdr *kernel_symtab = NULL;
static struct elf64_shdr *kernel_strtab = NULL;

void elf_set_kernel_info(struct boot_info *boot_info) {
	kernel_elf      = boot_info->kernel_file->addr;
	kernel_elf_size = boot_info->kernel_file->size;

	kernel_symtab = find_section(kernel_elf, ".symtab");
	kernel_strtab = find_section(kernel_elf, ".strtab");
}

const char *elf_get_kernel_symbol(void *addr) {
	extern char __etext;

	if (kernel_symtab == NULL || kernel_strtab == NULL) {
		return "";
	}

	const char *names            = (void *) kernel_elf + kernel_strtab->sh_offset;
	struct elf64_symbol *symbols = (void *) kernel_elf + kernel_symtab->sh_offset;

	u64 best_match = 0;
	int n_entries  = kernel_symtab->sh_size / kernel_symtab->sh_entsize;
	for (int i = 0; i < n_entries; i++) {
		struct elf64_symbol *sym = &symbols[i];

		bool in_text_section =
			sym->sym_value >= 0xffffffff80000000UL && sym->sym_value < (u64) &__etext;
		if (sym->sym_size == 0 && in_text_section) {
			if ((u64) addr - sym->sym_value < (u64) addr - symbols[best_match].sym_value) {
				best_match = i;
			}
			continue;
		}

		if (sym->sym_value <= (u64) addr && sym->sym_value + sym->sym_size >= (u64) addr) {
			return &names[sym->sym_name];
		}
	}

	return best_match == 0 ? "" : &names[symbols[best_match].sym_name];
}
