#pragma once

#include <common.h>
#include <boot/boot_info.h>

enum cpuid_flags {
	CPUID_FPU = 0,
	CPUID_VME = 1,
	CPUID_DE = 2,
	CPUID_PSE = 3,
	CPUID_TSC = 4,
	CPUID_MSR = 5,
	CPUID_PAE = 6,
	CPUID_MCE = 7,
	CPUID_CX8 = 8,
	CPUID_APIC = 9,
	CPUID_SEP = 11,
	CPUID_MTRR = 12,
	CPUID_PGE = 13,
	CPUID_MCA = 14,
	CPUID_CMOV = 15,
	CPUID_PAT = 16,
	CPUID_PSE36 = 17,
	CPUID_PSN = 18,
	CPUID_CLFSH = 19,
	CPUID_DS = 21,
	CPUID_ACPI = 22,
	CPUID_MMX = 23,
	CPUID_FXSR = 24,
	CPUID_SSE = 25,
	CPUID_SSE2 = 26,
	CPUID_SS = 27,
	CPUID_HTT = 28,
	CPUID_TM = 29,
	CPUID_IA64 = 30,
	CPUID_PBE = 31,
	CPUID_SSE3 = 32,
	CPUID_PCLMULQDQ = 33,
	CPUID_DTES64 = 34,
	CPUID_MONITOR = 35,
	CPUID_DS_CPL = 36,
	CPUID_VMX = 37,
	CPUID_SMX = 38,
	CPUID_EST = 39,
	CPUID_TM2 = 40,
	CPUID_SSSE3 = 41,
	CPUID_CNXTID = 42,
	CPUID_SDBG = 43,
	CPUID_FMA = 44,
	CPUID_CX16 = 45,
	CPUID_XPTR = 46,
	CPUID_PDCM = 47,
	CPUID_PCID = 49,
	CPUID_DCA = 50,
	CPUID_SSE41 = 51,
	CPUID_SSE42 = 52,
	CPUID_X2APIC = 53,
	CPUID_MOVBE = 54,
	CPUID_POPCNT = 55,
	CPUID_TSCDEADLINE = 56,
	CPUID_AES = 57,
	CPUID_XSAVE = 58,
	CPUID_OSXSAVE = 59,
	CPUID_AVX = 60,
	CPUID_F16C = 61,
	CPUID_RDRND = 62,
	CPUID_HYPERVISOR = 63,
	CPUID_SYSCALL = 75,
	CPUID_MP = 83,
	CPUID_NX = 84,
	CPUID_MMXEXT = 86,
	CPUID_FXSROPT = 89,
	CPUID_PDPE1GB = 90,
	CPUID_RDTSCP = 91,
	CPUID_LM = 93,
	CPUID_3DNOWEXT = 94,
	CPUID_3DNOW = 95,
	CPUID_LAHFLM = 96,
	CPUID_CMPLEGACY = 97,
	CPUID_SVM = 98,
	CPUID_EXTAPIC = 99,
	CPUID_CR8LEGACY = 100,
	CPUID_ABM = 101,
	CPUID_SSE4A = 102,
	CPUID_MISALIGNSSE = 103,
	CPUID_3DNOWPREFETCH = 104,
	CPUID_OSVW = 105,
	CPUID_IBS = 106,
	CPUID_XOP = 107,
	CPUID_SKINIT = 108,
	CPUID_WDT = 109,
	CPUID_LWP = 111,
	CPUID_FMA4 = 112,
	CPUID_TCE = 113,
	CPUID_NODEIDMSR = 115,
	CPUID_TBM = 117,
	CPUID_TOPOEXT = 118,
	CPUID_PERFCTRCORE = 119,
	CPUID_PERFCTRNB = 120,
	CPUID_DBX = 122,
	CPUID_PERFTSC = 123,
	CPUID_PCXL2I = 124,
	CPUID_MONITORX = 125,
	CPUID_ADDRMASKEXT = 126
};

void cpu_init(struct boot_info *boot_info);
bool cpu_has_flag(enum cpuid_flags flag);

bool cpuid(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx);

