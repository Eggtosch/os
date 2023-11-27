#include <cpu/cpu.h>
#include <io/stdio.h>
#include <memory/pmm.h>
#include <panic.h>
#include <string.h>

#define VIRTUAL_ADDR_OFFSET (0xffff800000000000UL)

struct mem_bitmap {
	u64 bitmap_size;
	u64 mem_size;
	u64 *mem_map;
	u64 mem_lowest_free_page;
};

static struct mem_info *_mem_info;
static struct mem_bitmap _bitmap;

static void mem_bitmap_set(u64 bit) {
	if (bit / 64 > _bitmap.mem_size) {
		return;
	}
	_bitmap.mem_map[bit / 64] |= ((u64) 1 << (bit % 64));
}

static void mem_bitmap_unset(u64 bit) {
	if (bit / 64 > _bitmap.mem_size) {
		return;
	}
	_bitmap.mem_map[bit / 64] &= ~((u64) 1 << (bit % 64));
}

static u8 mem_bitmap_check(u64 bit) {
	if (bit / 64 > _bitmap.mem_size) {
		return 1;
	}
	return (_bitmap.mem_map[bit / 64] >> (bit % 64)) & 1;
}

static const char *get_memtype(u32 mem_type) {
	switch (mem_type) {
	case MEM_USABLE:
		return "usable";
	case MEM_RESERVED:
		return "reserved";
	case MEM_ACPI_RECLAIMABLE:
		return "acpi1";
	case MEM_ACPI_NVS:
		return "acpi2";
	case MEM_BAD_MEMORY:
		return "bad memory";
	case MEM_BOOTLOADER_RECLAIMABLE:
		return "bootloader reclaimable";
	case MEM_KERNEL_AND_MODULES:
		return "kernel and modules";
	case MEM_FRAMEBUFFER:
		return "framebuffer";
	default:
		return "unknown";
	}
}

static char *to_unit(u64 bytes) {
	const char *units[] = {"B", "KB", "MB", "GB", "TB"};
	static char tmp[7];
	u64 unit = 0;
	while (bytes >= 1000) {
		bytes /= 1000;
		unit++;
	}
	snprintf(tmp, sizeof(tmp), "%.3d %s", bytes, units[unit]);
	return tmp;
}

static void cache_lowest_free_page_from(u64 start_page) {
	u64 npages = _bitmap.mem_size / PAGE_SIZE;
	for (u64 i = start_page; i < npages; i++) {
		if (mem_bitmap_check(i) == 0) {
			_bitmap.mem_lowest_free_page = i;
			return;
		}
	}
}

void pmm_init(struct boot_info *boot_info) {
	_mem_info                        = &boot_info->mem_info;
	u64 ram_top                      = 0;
	struct mem_entry *largest_region = _mem_info->mem_map[0];

	kprintf("memory map:\n");

	for (u64 i = 0; i < _mem_info->mem_entries; i++) {
		struct mem_entry *entry = _mem_info->mem_map[i];

		kprintf("    %#0.16x (%s): %s\n", entry->mem_base, to_unit(entry->mem_length),
		        get_memtype(entry->mem_type));

		if (entry->mem_type != MEM_USABLE) {
			continue;
		}

		u64 tmp = (u64) entry->mem_base + entry->mem_length;
		if (tmp > ram_top) {
			ram_top = tmp;
		}
		if (entry->mem_length > largest_region->mem_length) {
			largest_region = entry;
		}
	}

	_bitmap.bitmap_size = ram_top / PAGE_SIZE / 8;
	_bitmap.mem_size    = ram_top;
	_bitmap.mem_map     = NULL;

	for (u64 i = 0; i < _mem_info->mem_entries; i++) {
		struct mem_entry *entry = _mem_info->mem_map[i];

		if (entry->mem_type != MEM_USABLE) {
			continue;
		}

		if (entry->mem_length >= _bitmap.bitmap_size) {
			_bitmap.mem_map         = (u64 *) pmm_to_virt((u64) entry->mem_base);
			u64 aligned_bitmap_size = (_bitmap.bitmap_size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
			entry->mem_base += aligned_bitmap_size;
			entry->mem_length -= aligned_bitmap_size;
			break;
		}
	}

	if (_bitmap.mem_map == NULL) {
		panic("could not find a memory region big enough for the pmm bitmap");
	}

	for (u64 i = 0; i < _bitmap.bitmap_size / 8; i++) {
		_bitmap.mem_map[i] = 0xffffffffffffffff;
	}

	for (u64 i = 0; i < _mem_info->mem_entries; i++) {
		struct mem_entry *entry = _mem_info->mem_map[i];
		if (entry->mem_type == MEM_USABLE) {
			pmm_free(entry->mem_base, entry->mem_length / PAGE_SIZE);
		}
	}

	mem_bitmap_set(0);

	cache_lowest_free_page_from(0);

	struct mem_entry *last_entry = _mem_info->mem_map[_mem_info->mem_entries - 1];
	u64 total_bytes              = (u64) last_entry->mem_base + last_entry->mem_length;
	const char *total_bytes_str  = to_unit(total_bytes);
	while (*total_bytes_str == ' ') {
		total_bytes_str++;
	}
	kprintf("total system ram: %u (%s) bytes\n", total_bytes, total_bytes_str);
}

void *pmm_alloc(u64 page_count) {
	if (page_count == 0) {
		return 0;
	}
	u64 consecutive_free_pages = 0;
	u64 npages                 = _bitmap.mem_size / PAGE_SIZE;
	for (u64 i = _bitmap.mem_lowest_free_page; i < npages; i++) {
		if (mem_bitmap_check(i) == 0) {
			consecutive_free_pages++;
		} else {
			consecutive_free_pages = 0;
		}
		if (consecutive_free_pages >= page_count) {
			for (u64 marked_pages = 0; marked_pages < page_count; marked_pages++) {
				mem_bitmap_set(i - marked_pages);
			}
			u64 start_page = i - page_count + 1;
			if (start_page == _bitmap.mem_lowest_free_page) {
				cache_lowest_free_page_from(i);
			}
			void *ptr = pmm_to_virt(start_page * PAGE_SIZE);
			memset(ptr, 0, page_count * PAGE_SIZE);
			return ptr;
		}
	}
	panic("out of memory: requested %d pages", page_count);
}

void pmm_free(void *ptr, u64 page_count) {
	u64 index = (u64) pmm_to_phys(ptr) / PAGE_SIZE;
	for (u64 i = 0; i < page_count; i++) {
		mem_bitmap_unset(index + i);
	}
	if (index <= _bitmap.mem_lowest_free_page) {
		_bitmap.mem_lowest_free_page = index;
	}
}

static u64 popcnt_fallback(u64 x) {
	u64 cnt = 0;
	while (x) {
		cnt += x & 1;
		x >>= 1;
	}
	return cnt;
}

u64 pmm_get_free_bytes(void) {
	u64 bytes_free = 0;
	if (cpu_has_flag(CPUID_POPCNT)) {
		for (u64 i = 0; i < _bitmap.bitmap_size / 8; i++) {
			u64 used_pages = 0;
			asm("popcnt %0, %1" : "=r"(used_pages) : "r"(_bitmap.mem_map[i]));
			bytes_free += (64 - used_pages) * PAGE_SIZE;
		}
	} else {
		for (u64 i = 0; i < _bitmap.bitmap_size / 8; i++) {
			u64 used_pages = popcnt_fallback(_bitmap.mem_map[i]);
			bytes_free += (64 - used_pages) * PAGE_SIZE;
		}
	}
	return bytes_free;
}

u64 pmm_to_phys(void *vaddr) {
	u64 paddr = (u64) vaddr;
	if (paddr < VIRTUAL_ADDR_OFFSET) {
		return paddr;
	}
	return paddr - VIRTUAL_ADDR_OFFSET;
}

void *pmm_to_virt(u64 paddr) {
	void *vaddr = (void *) paddr;
	if (paddr >= VIRTUAL_ADDR_OFFSET) {
		return vaddr;
	}
	return vaddr + VIRTUAL_ADDR_OFFSET;
}

void *pmm_highest_memmap_addr(void) {
	u64 highest = 0;
	for (u64 i = 0; i < _mem_info->mem_entries; i++) {
		struct mem_entry *entry = _mem_info->mem_map[i];
		u64 addr                = (u64) entry->mem_base + entry->mem_length;
		if (addr > highest) {
			highest = addr;
		}
	}

	if (highest <= 0xffffffffUL) {
		highest = 0x100000000UL;
	}

	highest |= 0x7fffffffUL;
	highest += 1;

	return (void *) (highest + VIRTUAL_ADDR_OFFSET);
}

u64 pmm_framebuffer_addr(void) {
	for (u64 i = 0; i < _mem_info->mem_entries; i++) {
		struct mem_entry *entry = _mem_info->mem_map[i];
		if (entry->mem_type == MEM_FRAMEBUFFER) {
			return (u64) entry->mem_base;
		}
	}

	return 0;
}
