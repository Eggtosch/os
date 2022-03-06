#include <device/keyboard.h>
#include <interrupts/interrupts.h>
#include <device/pic.h>
#include <io/io.h>
#include <debug.h>
#include <io/stdio.h>


#define KEYBOARD_DATA (0x60)
#define KEYBOARD_CMD  (0x64)


static char _scancodes[] = {
	0, 0,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '?', '`', '\b',
	'\t','q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'u', '+', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'o', 'a', '<',
	0, '#', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
	0, 0, ' ', 0, ' ', 0, 0, 0, 0, 0, 0,
	[0x56] = '<'
};


static void isr_keyboard(struct cpu_state *cpu_state) {
	(void) cpu_state;
	u8 scancode = io_inb(KEYBOARD_DATA);
	char key = keyboard_scancode_to_ascii(scancode);
	if (!(scancode & 0x80)) {
		printf("%c", key);
	}
}

char keyboard_scancode_to_ascii(u8 scancode) {
	return _scancodes[scancode & 0x7f];
}

void keyboard_init(void) {
	interrupt_register(INT_KEYBOARD, isr_keyboard, INT_KERNEL);

	while (io_inb(KEYBOARD_CMD) & 1) {
		io_inb(KEYBOARD_DATA);
	}

	pic_clear_mask(INT_KEYBOARD);
	debug(DEBUG_INFO, "Initialized keyboard driver");
}
