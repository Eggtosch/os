#include <cpu/cpu.h>
#include <acpi/acpi.h>
#include <io/io.h>
#include <string.h>

struct cpuid_info {
	u32 max_basic_leaf;
	u32 max_ext_leaf;
	char man_id[12];
	u64 basic_flags;
	u64 ext_flags;
};

static struct cpuid_info cpuid_info;

void cpu_init(__unused struct boot_info *boot_info) {
	char man_id_1[4], man_id_2[4], man_id_3[4];
	asm volatile("cpuid" : "=a"(cpuid_info.max_basic_leaf), "=b"(man_id_1), "=d"(man_id_2), "=c"(man_id_3) : "a"(0));
	asm volatile("cpuid" : "=a"(cpuid_info.max_ext_leaf) : "a"(0x80000000) : "ebx", "ecx", "edx");
	memcpy(cpuid_info.man_id + 0, man_id_1, 4);
	memcpy(cpuid_info.man_id + 4, man_id_2, 4);
	memcpy(cpuid_info.man_id + 8, man_id_3, 4);

	u32 basic_edx, basic_ecx;
	u32 ext_edx, ext_ecx;
	asm volatile("cpuid" : "=d"(basic_edx), "=c"(basic_ecx) : "a"(1) : "ebx");
	asm volatile("cpuid" : "=d"(ext_edx), "=c"(ext_ecx) : "a"(0x80000001) : "ebx");
	cpuid_info.basic_flags = (u64) basic_ecx << 32 | basic_edx;
	cpuid_info.ext_flags = (u64) ext_ecx << 32 | ext_edx;

	kprintf("saved available cpu features\n");
}

bool cpu_has_flag(enum cpuid_flags flag) {
	if (flag < 64) {
		return (cpuid_info.basic_flags & (1UL << flag)) != 0;
	}
	return (cpuid_info.ext_flags & (1UL << (flag - 64))) != 0;
}

void cpu_reset(void) {
	acpi_reset();
	io_outw(0x604, 0x2000);
}

bool cpuid(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx) {
	u32 cpuid_max;
	asm volatile("cpuid" : "=a"(cpuid_max) : "a"(leaf & 0x80000000) :
						   "ebx", "ecx", "edx");
	if (leaf > cpuid_max)
		return false;
	asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) :
						   "a"(leaf), "c"(subleaf));
	return true;
}

