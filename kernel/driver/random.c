#include <driver/driver.h>
#include <cpu/cpu.h>

static u64 random_read(__unused struct io_device *dev, u8 *buf, u64 buflen, __unused u64 offset) {
	u64 val = 0;
	u64 checksum = 0;
	u64 index = 0;

	while (index + 8 < buflen) {
		asm volatile("rdrand %0" : "=r"(val));
		*(u64*) (buf + index) = val;
		checksum += (val == 0);
		index += 8;
	}

	asm volatile("rdrand %0" : "=r"(val));
	checksum += (val == 0);
	for (; index < buflen; index++) {
		buf[index] = val & 0xff;
		val >>= 8;
	}

	return checksum != 0 ? 0 : buflen;
}

static struct driver_file driver_files[] = {
	{"/dev/random", {random_read, NULL, NULL}},
	{NULL, {NULL, NULL, NULL}},
};

static void random_init(__unused struct boot_info *boot_info) {
	if (cpu_has_flag(CPUID_RDRND)) {
		driver_register("random", driver_files);
	}
}

driver_init("random", random_init);

