#include <framebuffer/framebuffer.h>
#include <io/io.h>
#include <cpu/cpu.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <string.h>

static struct fb_info _fb_info;

static bool remap_framebuffer(void) {
	void *virt_addr = pmm_highest_memmap_addr();
	u64 phys_fb_addr = pmm_framebuffer_addr();
	u64 size = _fb_info.fb_pitch * _fb_info.fb_height * sizeof(u32);
	if (phys_fb_addr == 0) {
		return false;
	}

	u64 pat_msr = rdmsr(MSR_PAT);
	pat_msr |= (1UL << 32);
	pat_msr &= ~(1UL << 33);
	pat_msr &= ~(1UL << 34);
	wrmsr(MSR_PAT, pat_msr);

	vmm_map_existing(NULL, virt_addr, phys_fb_addr, size);
	_fb_info.fb_addr = virt_addr;
	vmm_set_wc(NULL, virt_addr, size);

	return true;
}

static bool fb_in_range(struct fb_info *fb_info, u16 x, u16 y, u16 width, u16 height) {
	return (x + width <= fb_info->fb_width) && 
		   (y + height <= fb_info->fb_height);
}

void framebuffer_init(struct boot_info *boot_info) {
	_fb_info = boot_info->fb_info;
	_fb_info.fb_pitch /= sizeof(u32);

	kprintf("initialized framebuffer: %dx%d (32 bpp, ARGB format)\n", _fb_info.fb_width, _fb_info.fb_height);

	if (remap_framebuffer()) {
		kprintf("framebuffer vmm write combining enabled\n");
	}
}

i64 framebuffer_read(u16 x, u16 y, u16 width, u16 height, u32 *buffer) {
	if (!fb_in_range(&_fb_info, x, y, width, height)) {
		return -1;
	}

	u32 *src = _fb_info.fb_addr + (y * _fb_info.fb_pitch) + x;
	u32 *dst = (u32*) buffer;
	for (u64 _y = 0; _y < height; _y++) {
		memcpy(dst, src, width * sizeof(u32));
		dst += width;
		src += _fb_info.fb_pitch;
	}

	return (u64) dst - (u64) buffer;
}

i64 framebuffer_write(u16 x, u16 y, u16 width, u16 height, u32 *buffer) {
	if (!fb_in_range(&_fb_info, x, y, width, height)) {
		return -1;
	}

	u32 *dst = _fb_info.fb_addr + y * _fb_info.fb_pitch + x;
	u32 *src = (u32*) buffer;

	for (u64 _y = 0; _y < height; _y++) {
		memcpy(dst, src, width * sizeof(u32));
		src += width;
		dst += _fb_info.fb_pitch;
	}

	return (u64) src - (u64) buffer;
}
