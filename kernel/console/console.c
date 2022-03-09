#include <console/console.h>
#include <device/framebuffer.h>
#include <debug.h>
#include <device/serial.h>

#include <console/font.gen.h>

struct console_io {
	struct io_device header;
	struct fb_buffer fb;
	struct font *font;
	u16 x_pos;
	u16 y_pos;
	u32 fgcolor;
	u32 bgcolor;
	bool flush_on_newline;
};

static struct console_io _console_io_impl;
struct io_device *console_io;


static u64 console_read(struct io_device *stream, u8 *buf, u64 bufsize) {
	(void) stream; // keyboard interrupt
	(void) buf;
	(void) bufsize;
	return 0;
}

static void console_putc(struct io_device *stream, u8 c);
static void handle_control_char(struct io_device *stream, u8 c) {
	struct console_io *_io = (struct console_io*) stream;
	if (c == '\n') {
		_io->x_pos = 0;
		_io->y_pos += _io->font->font_height;
		if (_io->flush_on_newline) {
			console_flush(stream);
		}
		return;
	} else if (c == '\r') {
		_io->x_pos = 0;
		return;
	} else if (c == '\t') {
		for (u8 i = 0; i < 4; i++) {
			console_putc(stream, ' ');
		}
		return;
	} else if (c == '\b') {
		if (_io->x_pos < _io->font->font_width) {
			return;
		}
		_io->x_pos -= _io->font->font_width;
		console_putc(stream, ' ');
		_io->x_pos -= _io->font->font_width;
		return;
	}
}

static void console_putc(struct io_device *stream, u8 c) {
	struct console_io *_io = (struct console_io*) stream;
	if (c < ' ') {
		handle_control_char(stream, c);
		return;
	}

	if (_io->x_pos + _io->font->font_width >= _io->fb.width) {
		_io->x_pos = 0;
		_io->y_pos += _io->font->font_height;
	}

	if (_io->y_pos + _io->font->font_height >= _io->fb.height) {
		u16 to_scroll = _io->fb.height / _io->font->font_height;
		if (to_scroll > 4) {
			to_scroll = 4;
		}
		framebuffer_scroll_down(_io->fb, _io->font->font_height * to_scroll, _io->bgcolor);
		_io->y_pos -= _io->font->font_height * to_scroll;
	}

	struct fb_rect dst = {
		_io->x_pos,
		_io->y_pos,
		_io->font->font_width,
		_io->font->font_height
	};
	u8 *glyph = _io->font->font_data + c * _io->font->font_height;
	framebuffer_draw_glyph(_io->fb, glyph, &dst, _io->fgcolor, _io->bgcolor);

	_io->x_pos += _io->font->font_width;
}

static u64 console_write(struct io_device *stream, u8 *buf, u64 count) {
	u64 i;
	for (i = 0; i < count; i++) {
		console_putc(stream, buf[i]);
	}
	return i;
}

void console_init(void) {
	struct console_io *_io = (struct console_io*) &_console_io_impl;
	_io->font              = &fonts[1];
	_io->header.read       = console_read;
	_io->header.write      = console_write;
	_io->fb                = framebuffer_unbuffered();
	_io->fgcolor           = COLOR_WHITE;
	_io->bgcolor           = COLOR_BLACK;
	_io->flush_on_newline  = true;
	console_io             = (struct io_device*) _io;
}

void console_init_buffering(struct io_device *stream) {
	struct console_io *_io = (struct console_io*) stream;
	_io->fb = framebuffer_init_buffer(0, 0, 10000, 10000);
	debug(DEBUG_INFO, "Initialized double buffering for framebuffer");
}

void console_set_fgcolor(struct io_device *stream, u32 color) {
	struct console_io *_io = (struct console_io*) stream;
	_io->fgcolor = color;
}

void console_set_bgcolor(struct io_device *stream, u32 color) {
	struct console_io *_io = (struct console_io*) stream;
	_io->bgcolor = color;
}

void console_flush_on_newline(struct io_device *stream, bool flush) {
	struct console_io *_io = (struct console_io*) stream;
	_io->flush_on_newline = flush;
}

void console_flush(struct io_device *stream) {
	struct console_io *_io = (struct console_io*) stream;
	framebuffer_present(_io->fb);
}

void console_clear(struct io_device *stream) {
	struct console_io *_io = (struct console_io*) stream;
	framebuffer_clear(_io->fb, _io->bgcolor);
	framebuffer_present(_io->fb);
	_io->x_pos = 0;
	_io->y_pos = 0;
}
