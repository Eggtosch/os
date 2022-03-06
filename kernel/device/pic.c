#include <device/pic.h>
#include <io/io.h>


#define PIC1_CMD  (0x20)
#define PIC2_CMD  (0xA0)
#define PIC1_DATA (PIC1_CMD + 1)
#define PIC2_DATA (PIC2_CMD + 1)

void pic_remap(void) {
	u8 mask1 = io_inb(PIC1_DATA);
	u8 mask2 = io_inb(PIC2_DATA);
	io_wait();

	io_outb(PIC1_CMD, 0x11);
	io_outb(PIC2_CMD, 0x11);
	io_wait();
	io_outb(PIC1_DATA, 0x20);
	io_outb(PIC2_DATA, 0x28);
	io_wait();
	io_outb(PIC1_DATA, 0x04);
	io_outb(PIC2_DATA, 0x02);
	io_wait();
	io_outb(PIC1_DATA, 0x01);
	io_outb(PIC2_DATA, 0x01);
	io_wait();
	io_outb(PIC1_DATA, 0x00);
	io_outb(PIC2_DATA, 0x00);
	io_wait();

	io_outb(PIC1_DATA, 0x01);
	io_wait();
	io_outb(PIC2_DATA, 0x01);
	io_wait();

	io_outb(PIC1_DATA, mask1);
	io_outb(PIC2_DATA, mask2);
}

void pic_set_mask(u8 isr) {
	u16 port;

	isr -= 32;
	if (isr < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		isr -= 8;
	}

	u8 value = io_inb(port) | (1 << isr);
	io_outb(port, value);
}

void pic_clear_mask(u8 isr) {
	u16 port;

	isr -= 32;
	if (isr < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		isr -= 8;
	}

	u8 value = io_inb(port) & ~(1 << isr);
	io_outb(port, value);
}

void pic_signal_eoi(u64 isr) {
	if (isr >= 40) {
		io_outb(PIC2_CMD, 0x20);
	}
	io_outb(PIC1_CMD, 0x20);
}
