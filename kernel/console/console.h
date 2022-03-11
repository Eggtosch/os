#pragma once

#include <io/stdio.h>


extern struct io_device *console_io;

#define COLOR_BLACK  (0x00000000)
#define COLOR_WHITE  (0x00ffffff)
#define COLOR_RED    (0x00ff0000)
#define COLOR_GREEN  (0x0000ff00)
#define COLOR_BLUE   (0x000000ff)
#define COLOR_YELLOW (0x00ffff00)

void console_init(void);
void console_init_buffering(struct io_device *stream);
void console_set_fgcolor(struct io_device *stream, u32 color);
void console_set_bgcolor(struct io_device *stream, u32 color);
void console_flush_on_newline(struct io_device *stream, bool flush);
void console_flush(struct io_device *stream);
void console_clear(struct io_device *stream);
