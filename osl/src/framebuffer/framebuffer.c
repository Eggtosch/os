#include <framebuffer/framebuffer.h>
#include <syscall.h>

#include <framebuffer/font.gen.h>


#define PIXEL_CHANGED 		((u32) 1 << 31)
#define DOUBLEPIXEL_CHANGED ((u64) 1 << 63 | (u64) 1 << 31)


int fb_init(struct fb_buffer *buffer) {
	return sys_fb_init(buffer);
}

int fb_deinit(struct fb_buffer *buffer) {
	return sys_fb_deinit(buffer);
}

int fb_clear(struct fb_buffer *buffer, u32 color) {
	u64 *dst = (u64*) buffer->buffer;
	u64 size = buffer->width * buffer->height / 2;
	color &= 0x00ffffff;
	u64 dcolor = (u64) color << 32 | color | DOUBLEPIXEL_CHANGED;
	for (u64 i = 0; i < size; i++) {
		dst[i] = dcolor;
	}
	return 0;
}

int fb_scroll_down(struct fb_buffer *buffer, u64 start_y, u32 bg_color) {
	bg_color &= 0x00ffffff;
	
	if (start_y > buffer->height) {
		fb_clear(buffer, bg_color);
		return 0;
	}

	for (u64 line = start_y; line < buffer->height; line++) {
		u64 *src = (u64*) (buffer->buffer + (line * buffer->width));
		u64 *dst = (u64*) (buffer->buffer + (line - start_y) * buffer->width);
		for (u64 x = 0; x < buffer->width / 2; x++) {
			if (dst[x] != src[x]) {
				dst[x] = src[x] | DOUBLEPIXEL_CHANGED;
			}
		}
	}

	u64 color64 = (u64) bg_color << 32 | bg_color | DOUBLEPIXEL_CHANGED;
	u64 *dst = (u64*) (buffer->buffer + (buffer->height - start_y) * buffer->width);
	for (u64 line = buffer->height - start_y; line < buffer->height; line++) {
		for (u64 x = 0; x < buffer->width / 2; x++) {
			dst[x] = color64;
		}
		dst += buffer->width / 2;
	}
	return 0;
}

int fb_draw_glyph(struct fb_buffer *buffer, u8 *glyph, struct fb_rect *dst, u32 fg, u32 bg) {
	u32 width = buffer->width;
	u32 *dest = buffer->buffer + (dst->y * width) + dst->x;
	u32 dheight = dst->height;
	u32 dwidth  = dst->width;

	for (u32 y = 0; y < dheight; y++) {
		u64 data = glyph[y];

		u32 x = dwidth - 1;
		do {
			u32 color = data & 1 ? fg : bg;
			dest[x] = color | PIXEL_CHANGED;
			data >>= 1;
		} while(x-- > 0);
		dest += width;
	}
	return 0;
}

int fb_present(struct fb_buffer *buffer) {
	return sys_fb_present(buffer);
}
