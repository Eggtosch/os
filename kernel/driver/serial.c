#include <io/io.h>
#include <driver/driver.h>

#define COM1 0x3f8


static bool serial_recv_ready(void) {
	return io_inb(COM1 + 5) & 1;
}

static bool serial_send_ready(void) {
	return io_inb(COM1 + 5) & 0x20;
}

static u64 serial_recv(struct io_device *stream, u8 *buf, u64 bufsize) {
	(void) stream;
	u64 i;
	for (i = 0; i < bufsize; i++) {
		while (!serial_recv_ready());
		buf[i] = io_inb(COM1);
		if (buf[i] == '\n') {
			break;
		}
	}
	return i;
}

static u64 serial_send(struct io_device *stream, u8 *buf, u64 count) {
	(void) stream;
	u64 i;
	for (i = 0; i < count; i++) {
		while (!serial_send_ready());
		io_outb(COM1, buf[i]);
	}
	return i;
}

static struct driver_file driver_files[] = {
	{"/dev/serial", {serial_recv, serial_send, NULL}},
	{NULL, {NULL, NULL, NULL}}
};

struct io_device *serial_get_io_device(void) {
	return &driver_files[0].stream;
}

static void serial_init(struct boot_info *boot_info) {
	(void) boot_info;
	io_outb(COM1 + 1, 0x00);
	io_outb(COM1 + 3, 0x80);
	io_outb(COM1 + 0, 0x03);
	io_outb(COM1 + 1, 0x00);
	io_outb(COM1 + 3, 0x03);
	io_outb(COM1 + 2, 0xC7);
	io_outb(COM1 + 4, 0x0B);
	driver_register("serial", driver_files);
}

driver_init("serial", serial_init);
