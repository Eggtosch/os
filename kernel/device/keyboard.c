#include <device/keyboard.h>

#include <interrupts/interrupts.h>
#include <device/pic.h>
#include <io/io.h>
#include <vfs/vfs.h>

#include <debug.h>


#define KEYBOARD_DATA (0x60)
#define KEYBOARD_CMD  (0x64)

#define MAX_SCANCODES 1024

static struct io_device _keyboard_io;
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


static u64 keyboard_read(struct io_device *stream, u8 *buf, u64 size) {
	(void) stream;
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

static u64 keyboard_write(struct io_device *stream, u8 *buf, u64 size) {
	(void) buf;
	(void) size;
	(void) stream;
	return 0;
}

static void isr_keyboard(struct cpu_state *cpu_state) {
	(void) cpu_state;
	if (_scancode_count >= MAX_SCANCODES) {
		return;
	}
	_scancodes[_scancode_count] = io_inb(KEYBOARD_DATA);
	_scancode_count++;
}

char keyboard_scancode_to_ascii(u8 scancode) {
	return _scancodes[scancode & 0x7f];
}

void keyboard_init(void) {
	interrupt_register(INT_KEYBOARD, isr_keyboard, INT_KERNEL);

	while (io_inb(KEYBOARD_CMD) & 1) {
		io_inb(KEYBOARD_DATA);
	}

	_keyboard_io.read  = keyboard_read;
	_keyboard_io.write = keyboard_write;
	_scancode_count = 0;
	vfs_mount("/dev/keyboard", &_keyboard_io);

	pic_clear_mask(INT_KEYBOARD);
	debug(DEBUG_INFO, "Initialized keyboard driver");
}
