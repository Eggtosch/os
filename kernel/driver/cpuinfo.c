#include <driver/driver.h>
#include <cpu/cpu.h>
#include <memory/pmm.h>
#include <string.h>

static const char *cpu_flag_names[] = {
	"fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
	"cx8", "apic", "-", "sep", "mtrr", "pge", "mca", "cmov",
	"pat", "pse-36", "psn", "clfsh", "-", "ds", "acpi", "mmx",
	"fxsr", "sse", "sse2", "ss", "htt", "tm", "ia64", "pbe",
	"sse3", "pclmulqdq", "dtes64", "monitor", "ds-cpl", "vmx", "smx", "est",
	"tm2", "ssse3", "cnxt-id", "sdbg", "fma", "cx16", "xptr", "pdcm",
	"-", "pcid", "dca", "sse4.1", "sse4.2", "x2apic", "movbe", "popcnt",
	"tsc-deadline", "aes", "xsave", "osxsave", "avx", "f16c", "rdrnd", "hypervisor",
	"fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
	"cx8", "apic", "-", "syscall", "mtrr", "pge", "mca", "cmov",
	"pat", "pse36", "-", "mp", "nx", "-", "mmxext", "mmx",
	"fxsr", "fxsr_opt", "pdpe1gb", "rdtscp", "-", "lm", "3dnowext", "3dnow",
	"lahf_lm", "cmp_legacy", "svm", "extapic", "cr8_legacy", "abm", "sse4a", "misalignsse",
	"3dnowprefetch", "osvw", "ibs", "xop", "skinit", "wdt", "-", "lwp",
	"fma4", "tce", "-", "nodeid_msr", "-", "tbm", "topoext", "perfctr_core",
	"perfctr_nb", "-", "dbx", "perftsc", "pcx_l2i", "monitorx", "addr_mask_ext", "-"
};

static const char *cpuinfo_string;
static u64 cpuinfo_string_len;

static u64 cpuinfo_read(struct io_device *dev, u8 *buf, u64 buflen) {
	(void) dev;
	u64 max = buflen > cpuinfo_string_len ? buflen : cpuinfo_string_len;
	memcpy(buf, cpuinfo_string, max);
	return max;
}

static struct driver_file driver_files[] = {
	{"/dev/cpuinfo", {cpuinfo_read, NULL, NULL}},
	{NULL, {NULL, NULL, NULL}},
};

static void resize(char **s, u64 *n_pages) {
	char *new = (void*) pmm_alloc(*n_pages + 1);
	memcpy(new, *s, *n_pages * PAGE_SIZE);
	pmm_free((u64) *s, *n_pages);
	*s = new;
	(*n_pages)++;
}

static void cpuinfo_init(struct boot_info *boot_info) {
	(void) boot_info;

	u64 pages = 1;
	char *s = (void*) pmm_alloc(pages);
	u64 index = 0;
	for (u64 flag = 0; flag < ARRAY_LEN(cpu_flag_names); flag++) {
		if (!cpu_has_flag(flag)) {
			continue;
		}

		if (pages * PAGE_SIZE < index) {
			resize(&s, &pages);
		}

		u64 copied = strncpy_s(s + index, cpu_flag_names[flag], (pages * PAGE_SIZE) - index);
		if (copied == 0) {
			resize(&s, &pages);
			flag--;
			continue;
		}
		index += copied;

		if (index + 1 > pages * PAGE_SIZE) {
			resize(&s, &pages);
		}
		s[index++] = ' ';
	}

	cpuinfo_string = s;
	cpuinfo_string_len = index + 1;

	driver_register("cpuinfo", driver_files);
}

driver_init("cpuinfo", cpuinfo_init);

