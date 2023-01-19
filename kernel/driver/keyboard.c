#include <driver/driver.h>
#include <interrupts/interrupts.h>
#include <io/io.h>


#define KEYBOARD_DATA (0x60)
#define KEYBOARD_CMD  (0x64)

#define MAX_SCANCODES 1024

static u8  _scancode_buffer[MAX_SCANCODES];
static u64 _scancode_count;

static char _scancodes[] = {
	0, 0,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '?', '`', '\b',
	'\t','q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'u', '+', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'o', 'a', '<',
	0, '#', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
	0, 0, ' ', 0, ' ', 0, 0, 0, 0, 0, 0,
	[0x56] = '<'
};


static u64 keyboard_read(__unused struct io_device *stream, u8 *buf, u64 size, __unused u64 offset) {
	u64 i = 0;
	for (; i < size && i < _scancode_count; i++) {
		buf[i] = _scancode_buffer[i];
	}

	u64 copy_from = i;
	u64 copy_to = 0;
	for (; copy_to < i; copy_to++, copy_from++) {
		_scancode_buffer[copy_to] = _scancode_buffer[copy_from];
	}

	_scancode_count -= i;

	return i;
}

static void isr_keyboard(__unused struct cpu_state *cpu_state) {
	if (_scancode_count >= MAX_SCANCODES) {
		return;
	}
	_scancodes[_scancode_count] = io_inb(KEYBOARD_DATA);
	_scancode_count++;
}

static struct driver_file driver_files[] = {
	{"/dev/keyboard", {keyboard_read, NULL, NULL, NULL}},
	{NULL, {NULL, NULL, NULL, NULL}}
};

static void keyboard_init(__unused struct boot_info *boot_info) {
	interrupt_register(INT_KEYBOARD, isr_keyboard, INT_KERNEL);

	while (io_inb(KEYBOARD_CMD) & 1) {
		io_inb(KEYBOARD_DATA);
	}

	_scancode_count = 0;

	interrupt_enable(INT_KEYBOARD, true);
	driver_register("keyboard", driver_files);
}

driver_init("keyboard", keyboard_init);

