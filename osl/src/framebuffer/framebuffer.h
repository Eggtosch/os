#pragma once

#include <syscall.h>

struct fb_rect {
	u16 x;
	u16 y;
	u16 width;
	u16 height;
};

int fb_init(struct fb_buffer *buffer);
int fb_deinit(struct fb_buffer *buffer);
int fb_clear(struct fb_buffer *buffer, u32 color);
int fb_scroll_down(struct fb_buffer *buffer, u64 start_y, u32 bg_color);
int fb_draw_glyph(struct fb_buffer *buffer, u8 *glyph, struct fb_rect *dst, u32 fg, u32 bg);
int fb_present(struct fb_buffer *buffer);
