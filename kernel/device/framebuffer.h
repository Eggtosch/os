#pragma once

#include <boot/boot_info.h>
#include <stdio.h>

struct fb_rect {
	u16 x;
	u16 y;
	u16 width;
	u16 height;
};

struct fb_buffer {
	u32 *buffer;
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	u16 pitch;
};

u64 framebuffer_init(struct fb_info *fb_info);

struct fb_buffer framebuffer_unbuffered(void);
struct fb_buffer framebuffer_init_buffer(u16 x, u16 y, u16 width, u16 height);
void framebuffer_deinit_buffer(struct fb_buffer buf);

void framebuffer_scroll_down(struct fb_buffer buf, u64 start_y, u32 bg_color);
void framebuffer_draw_glyph(struct fb_buffer buf, u8 *glyph, struct fb_rect *dst, u32 fg, u32 bg);
void framebuffer_present(struct fb_buffer buf);
void framebuffer_clear(struct fb_buffer buf, u32 color);
