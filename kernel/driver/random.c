#include <driver/driver.h>

static u64 random_read(struct io_device *dev, u8 *buf, u64 buflen) {
	(void) dev;
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
	//{"/dev/random", {random_read, NULL, NULL}},
	{NULL, {NULL, NULL, NULL}},
};

static void random_init(struct boot_info *boot_info) {
	(void) boot_info;
	driver_register("random", driver_files);
}

driver_init("random", random_init);

