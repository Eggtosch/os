#include <framebuffer/framebuffer.h>
#include <io/io.h>
#include <cpu/cpu.h>
#include <memory/pmm.h>

#define PIXEL_CHANGED ((u64) 1 << 31)

static struct fb_info _fb_info;

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
	for (u64 aligned = align;; aligned *= 2) {
		if (aligned >= value) {
			value = aligned;
			break;
		}
	}
	return value;
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
	u64 base = pmm_to_phys((u64) addr);
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

static bool fb_in_range(struct fb_info *fb_info, u16 x, u16 y, u16 width, u16 height) {
	return (x + width <= fb_info->fb_width) && 
		   (y + height <= fb_info->fb_height);
}

void framebuffer_init(struct boot_info *boot_info) {
	_fb_info = boot_info->fb_info;
	_fb_info.fb_pitch /= 4;

	u64 error = enable_mtrr(_fb_info.fb_addr, _fb_info.fb_pitch, _fb_info.fb_height);
	kprintf("initialized linear framebuffer: %dx%d (32 bpp, ARGB format)\n", _fb_info.fb_width, _fb_info.fb_height);
	kprintf("framebuffer mtrr write combining %ssupported\n", error ? "un" : "");
}

i64 framebuffer_read(u16 x, u16 y, u16 width, u16 height, u32 *buffer) {
	if (!fb_in_range(&_fb_info, x, y, width, height)) {
		return -1;
	}

	u32 *src = _fb_info.fb_addr + (y * _fb_info.fb_pitch) + x;
	u32 *dst = (u32*) buffer;
	for (u64 _y = 0; _y < height; _y++) {
		for (u64 _x = 0; _x < width; _x++) {
			*dst = src[x];
			dst++;
		}
		src += _fb_info.fb_pitch;
	}

	return (u64) dst - (u64) buffer;
}

i64 framebuffer_write(u16 x, u16 y, u16 width, u16 height, u32 *buffer) {
	if (!fb_in_range(&_fb_info, x, y, width, height)) {
		return -1;
	}

	u32 *dst = _fb_info.fb_addr + y * _fb_info.fb_pitch + x;
	u32 *src = (u32*) (buffer + 8);

	for (u64 _y = 0; _y < height; _y++) {
		for (u64 _x = 0; _x < width; _x++) {
			if (*src & PIXEL_CHANGED) {
				*src &= ~PIXEL_CHANGED;
				dst[_x] = *src;
			}
			src++;
		}
		dst += _fb_info.fb_pitch;
	}

	return (u64) src - (u64) buffer;
}

