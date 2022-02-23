#include <common.h>
#include <device/framebuffer.h>
#include <memory/pmm.h>
#include <debug.h>
#include <os/io.h>

#include <device/serial.h>

#define PIXEL_CHANGED 		((u32) 1 << 31)
#define DOUBLEPIXEL_CHANGED ((u64) 1 << 63 | (u64) 1 << 31)


static struct fb_info *_fb_info;


static u64 make_between(u64 min, u64 value, u64 max) {
	return (value < min) ? min : (value > max) ? max : value;
}

static u64 make_even(u64 value) {
	return value & (~1);
}

static bool mtrr_range_overlap(u64 base1, u64 mask1, u64 base2, u64 size2) {
	base1 &= ~0xfff;
	mask1 &= ~0xfff;

	for (u64 i = base2; i < size2; i += 4096) {
		if ((i & mask1) == (base1 & mask1))
			return true;
	}

	return false;
}

static u64 alignup_power2(u64 value, u64 align) {
	for (u64 aligned = 1;; aligned *= 2) {
		if (aligned >= value) {
			value = aligned;
			break;
		}
	}
	return ((value + align - 1) / align) * align;
}

static u64 enable_mtrr(u32 *addr, u64 pitch, u64 height) {
	u64 ia32_mtrrcap = rdmsr(0xfe);
	if (!(ia32_mtrrcap & (1 << 10))) {
		return 1;
	}

	u32 eax, ebx, ecx, edx;
	if (!cpuid(0x80000008, 0, &eax, &ebx, &ecx, &edx))
		return 1;

	u8 max_phys_addr = eax & 0xff;
	u64 base = VIRTUAL_TO_PHYSICAL((u64) addr);
	u64 size = alignup_power2(pitch * height, PAGE_SIZE);
	u64 mask = (((u64) 1 << max_phys_addr) - 1) & ~(size - 1);

	u8 nregs = ia32_mtrrcap & 0xff;
	for (u32 i = 0; i < nregs; i++) {
		u64 mtrrbase = rdmsr(0x200 + i * 2);
		u64 mtrrmask = rdmsr(0x200 + i * 2 + 1);
		if (mtrr_range_overlap(mtrrbase, mtrrmask, base, size)) {
			return 1;
		}
	}

	for (u8 i = 0; i < nregs; i++) {
		if (rdmsr(0x200 + i * 2 + 1) & (1 << 11))
			continue;

		wrmsr(0x200 + i * 2,     base | 0x1);
		wrmsr(0x200 + i * 2 + 1, mask | (1 << 11));
		return 0;
	}

	return 1;
}

u64 framebuffer_init(struct fb_info *fb_info) {
	_fb_info = fb_info;
	_fb_info->fb_pitch /= 4;

	return enable_mtrr(fb_info->fb_addr, fb_info->fb_pitch, fb_info->fb_height);
}

struct fb_buffer framebuffer_unbuffered(void) {
	struct fb_buffer buf;
	buf.buffer = _fb_info->fb_addr;
	buf.x = 0;
	buf.y = 0;
	buf.width  = _fb_info->fb_width;
	buf.height = _fb_info->fb_height;
	buf.pitch  = _fb_info->fb_pitch;
	return buf;
}

struct fb_buffer framebuffer_init_buffer(u16 x, u16 y, u16 width, u16 height) {
	struct fb_buffer buf;
	buf.x      = make_even(make_between(0, x, _fb_info->fb_width));
	buf.y      = make_even(make_between(0, y, _fb_info->fb_height));
	buf.width  = make_even(make_between(0, width,  _fb_info->fb_width  - buf.x));
	buf.height = make_even(make_between(0, height, _fb_info->fb_height - buf.y));
	buf.pitch  = buf.width;

	u64 size = buf.height * buf.width * 4;
	buf.buffer = pmm_alloc(size / PAGE_SIZE + 1);

	u64 *dst = (u64*) (buf.buffer);
	u64 *src = (u64*) (_fb_info->fb_addr + buf.y * _fb_info->fb_pitch + buf.x);
	for (u64 y = 0; y < buf.height; y++) {
		for (u64 x = 0; x < buf.width / 2; x++) {
			*dst = src[x];
			dst++;
		}
		src += _fb_info->fb_pitch / 2;
	}

	return buf;
}

void framebuffer_deinit_buffer(struct fb_buffer buf) {
	u64 size = buf.width * buf.height;

	u64 *src = (u64*) (buf.buffer);
	u64 *dst = (u64*) (_fb_info->fb_addr + buf.y * _fb_info->fb_pitch + buf.x);
	for (u64 y = buf.y; y < buf.y + buf.height; y++) {
		for (u64 x = buf.x; x < buf.x + buf.width / 2; x++) {
			dst[x] = *src;
			src++;
		}
		dst += _fb_info->fb_pitch / 2;
	}

	pmm_free(buf.buffer, size / PAGE_SIZE + 1);
}

void framebuffer_draw_glyph(struct fb_buffer buf, u8 *glyph, struct fb_rect *dst, u32 fg, u32 bg) {
	u32 pitch = buf.buffer == _fb_info->fb_addr ? _fb_info->fb_pitch : buf.width;
	u32 *dest = buf.buffer + (dst->y * pitch) + dst->x;
	u32 height = dst->height;
	u32 width  = dst->width;

	for (u32 y = 0; y < height; y++) {
		u64 data = glyph[y];

		u32 x = width - 1;
		do {
			u32 color = data & 1 ? fg : bg;
			dest[x] = color | PIXEL_CHANGED;
			data >>= 1;
		} while(x-- > 0);
		dest += pitch;
	}
}

void framebuffer_scroll_down(struct fb_buffer buf, u64 start_y, u32 bg_color) {
	if (start_y > buf.height) {
		framebuffer_clear(buf, bg_color);
		return;
	}

	for (u64 line = start_y; line < buf.height; line++) {
		u64 *src = (u64*) (buf.buffer + (line * buf.width));
		u64 *dst = (u64*) (buf.buffer + (line - start_y) * buf.width);
		for (u64 x = 0; x < buf.width / 2; x++) {
			if (dst[x] != src[x]) {
				dst[x] = src[x] | DOUBLEPIXEL_CHANGED;
			}
		}
	}

	u64 color64 = (u64) bg_color << 32 | bg_color | DOUBLEPIXEL_CHANGED;
	u64 *dst = (u64*) (buf.buffer + (buf.height - start_y) * buf.width);
	for (u64 line = buf.height - start_y; line < buf.height; line++) {
		for (u64 x = 0; x < buf.width / 2; x++) {
			dst[x] = color64;
		}
		dst += buf.width / 2;
	}
}

void framebuffer_present(struct fb_buffer buf) {
	if (buf.buffer == _fb_info->fb_addr) {
		return;
	}

	u64 *dst = (u64*) (_fb_info->fb_addr + buf.y * _fb_info->fb_pitch + buf.x);
	u64 *src = (u64*) (buf.buffer);
	for (u64 y = 0; y < buf.height; y++) {
		for (u64 x = 0; x < buf.width / 2; x++) {
			if (*src & DOUBLEPIXEL_CHANGED) {
				*src &= ~DOUBLEPIXEL_CHANGED;
				dst[x] = *src;
			}
			src++;
		}
		dst += _fb_info->fb_pitch / 2;
	}
	//asm volatile("rep movsq" :: "D"(dst), "S"(src), "c"(size/2) : "memory" );
}

void framebuffer_clear(struct fb_buffer buf, u32 color) {
	if (buf.buffer == _fb_info->fb_addr) {
		return;
	}

	u64 size = buf.height * buf.width;
	u64 *dst = (u64*) (buf.buffer);
	u64 color64 = (u64) color << 32 | color | DOUBLEPIXEL_CHANGED;
	for (u64 i = 0; i < size / 2; i++) {
		dst[i] = color64;
	}
}
