#include <memory/pmm.h>
#include <debug.h>
#include <stdio.h>
#include <kexit.h>


struct mem_bitmap {
	size_t bitmap_size;
	size_t mem_size;
	u64   *mem_map;
	u64    mem_lowest_free_page;
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
		case MEM_USABLE                : return "Usable";
		case MEM_RESERVED              : return "Reserved";
		case MEM_ACPI_RECLAIMABLE      : return "ACPI1";
		case MEM_ACPI_NVS              : return "ACPI2";
		case MEM_BAD_MEMORY            : return "Bad Memory";
		case MEM_BOOTLOADER_RECLAIMABLE: return "Bootloader Reclaimable";
		case MEM_KERNEL_AND_MODULES    : return "Kernel and Modules";
		case MEM_FRAMEBUFFER           : return "Framebuffer";
		default:                         return "Unknown";
	}
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

void pmm_init(struct mem_info *mem_info) {
	_mem_info = mem_info;
	size_t ram_top = 0;
	struct mem_entry *largest_region = &mem_info->mem_map[0];

	debug(DEBUG_INFO, "Memory Map:");

	for (u64 i = 0; i < mem_info->mem_entries; i++) {
		struct mem_entry *entry = &mem_info->mem_map[i];

		debug(DEBUG_INFO, "    %#0.16x (%s): %s", entry->mem_base, to_unit(entry->mem_length), get_memtype(entry->mem_type));

		if (entry->mem_type != MEM_USABLE) {
			continue;
		}

		size_t tmp = (uintptr_t) entry->mem_base + entry->mem_length;
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

	for (u64 i = 0; i < mem_info->mem_entries; i++) {
		struct mem_entry *entry = &mem_info->mem_map[i];

		if (entry->mem_type != MEM_USABLE) {
			continue;
		}

		if (entry->mem_length >= _bitmap.bitmap_size) {
			_bitmap.mem_map = (u64*) PHYSICAL_TO_VIRTUAL((uintptr_t) entry->mem_base);
			u64 aligned_bitmap_size = (_bitmap.bitmap_size + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
			entry->mem_base   += aligned_bitmap_size;
			entry->mem_length -= aligned_bitmap_size;
			break;
		}
	}

	if (_bitmap.mem_map == NULL) {
		debug(DEBUG_ERROR, "No memory region found big enough to store memory bitmap");
		kexit();
	}

	for (u64 i = 0; i < _bitmap.bitmap_size / 8; i++) {
		_bitmap.mem_map[i] = 0xffffffffffffffff;
	}

	for (u64 i = 0; i < mem_info->mem_entries; i++) {
		struct mem_entry *entry = &mem_info->mem_map[i];
		if (entry->mem_type == MEM_USABLE) {
			void *virtaddr = (void*) PHYSICAL_TO_VIRTUAL((uintptr_t) entry->mem_base);
			pmm_free(virtaddr, entry->mem_length / PAGE_SIZE);
		}
	}

	mem_bitmap_set(0);

	cache_lowest_free_page_from(0);

	debug(DEBUG_INFO, "Initialized PMM, Bitmap at: %p %d bytes", _bitmap.mem_map, _bitmap.bitmap_size);
}

void *pmm_alloc(size_t page_count) {
	if (page_count == 0) {
		return NULL;
	}
	u64 consecutive_free_pages = 0;
	u64 npages = _bitmap.mem_size / PAGE_SIZE;
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
			uintptr_t ptr = start_page * PAGE_SIZE;
			return (void*) PHYSICAL_TO_VIRTUAL(ptr);
		}
	}
	debug(DEBUG_INFO, "Out of memory!");
	kexit();
}

void pmm_free(void *ptr, size_t page_count) {
	u64 index = VIRTUAL_TO_PHYSICAL((uintptr_t) ptr) / PAGE_SIZE;
	for (u64 i = 0; i < page_count; i++) {
		mem_bitmap_unset(index + i);
	}
	if (index <= _bitmap.mem_lowest_free_page) {
		_bitmap.mem_lowest_free_page = index;
	}
}
