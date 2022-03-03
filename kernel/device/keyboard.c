#include <device/keyboard.h>
#include <interrupts/interrupts.h>
#include <device/pic.h>
#include <os/io.h>
#include <debug.h>


#define KEYBOARD_DATA (0x60)
#define KEYBOARD_CMD  (0x64)


//static char scancodes[128] = {
//};


static void isr_keyboard(struct cpu_state *cpu_state) {
	(void) cpu_state;
	u8 scancode = io_inb(KEYBOARD_DATA);
	debug(DEBUG_INFO, "got keyboard interrupt %#x", scancode);
}

void keyboard_init(void) {
	interrupt_register(INT_KEYBOARD, isr_keyboard, INT_KERNEL);

	while (io_inb(KEYBOARD_CMD) & 1) {
		io_inb(KEYBOARD_DATA);
	}

	pic_clear_mask(INT_KEYBOARD);
	debug(DEBUG_INFO, "Initialized keyboard driver");
}
