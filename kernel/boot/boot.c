#include <boot/boot_info.h>
#include <boot/stivale2.h>

#include <string.h>
#include <common.h>


static u8 os_stack[PAGE_SIZE * 4];


static struct stivale2_tag unmap_null_tag = {
	.identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID,
	.next = 0
};

static struct stivale2_header_tag_terminal boot_terminal_hdr_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
		.next = (u64) &unmap_null_tag
	},
	.flags = 0
};

static struct stivale2_header_tag_framebuffer boot_framebuffer_hdr_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		.next = (u64) &boot_terminal_hdr_tag
	},
	.framebuffer_width  = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp    = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header boot_stivale_hdr = {
	.entry_point = 0,
	.stack = (u64) os_stack + sizeof(os_stack),
	.flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
	.tags = (u64) &boot_framebuffer_hdr_tag
};


static void *get_stivale_struct(struct stivale2_struct *stivale2_info, u64 id) {
	struct stivale2_tag *current_tag = (void*) stivale2_info->tags;
	for (;;) {
		if (current_tag == NULL) {
			return NULL;
		}
		if (current_tag->identifier == id) {
			return current_tag;
		}
		current_tag = (void*) current_tag->next;
	}
}


static void (*write_err_ptr)(const char *string, u64 length) = NULL;

#define ERROR_HEADER  "Error: assert failed in '"
#define ERROR_HEADER2 "': ("
#define ERROR_TAIL    ")\n"
#define assert(cond, msg) _assert(cond, #cond, __func__, msg)
static void _assert(bool cond, const char *cond_str, const char *func_str, const char *msg) {
	if (cond || write_err_ptr == NULL) {
		return;
	}

	write_err_ptr(ERROR_HEADER, sizeof(ERROR_HEADER));
	write_err_ptr(func_str, strlen(func_str));
	write_err_ptr(ERROR_HEADER2, sizeof(ERROR_HEADER2));
	write_err_ptr(cond_str, strlen(cond_str));
	write_err_ptr(ERROR_TAIL, sizeof(ERROR_TAIL));

	if (msg == NULL) {
		return;
	}

	write_err_ptr(msg, strlen(msg));

	for (;;) {
		asm("hlt");
	}
}


static struct boot_info boot_info;

struct boot_info *boot_info_get(void) {
	return &boot_info;
}

extern void kmain(struct boot_info *boot_info);

void _start(struct stivale2_struct *stivale2_info) {
	struct stivale2_struct_tag_terminal *term_info;
	term_info = get_stivale_struct(stivale2_info, STIVALE2_STRUCT_TAG_TERMINAL_ID);
	if (term_info == NULL) {
		goto halt;
	}
	write_err_ptr = (void (*)(const char*, u64)) term_info->term_write;

	struct stivale2_struct_tag_framebuffer *fb_info;
	fb_info = get_stivale_struct(stivale2_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	assert(fb_info != NULL, "No framebuffer available!\n");
	assert(fb_info->framebuffer_bpp  == 32, "Kernel does only support 32 bpp framebuffer!\n");
	assert(fb_info->red_mask_size    ==  8 &&
		   fb_info->green_mask_size  ==  8 &&
		   fb_info->blue_mask_size   ==  8, "Kernel does only support 8 bit color masks!\n");
	assert(fb_info->red_mask_shift   == 16 &&
		   fb_info->green_mask_shift ==  8 &&
		   fb_info->blue_mask_shift  ==  0, "Kernel does only support RGB color format!\n");

	struct stivale2_struct_tag_memmap *mem_info;
	mem_info = get_stivale_struct(stivale2_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	assert(mem_info != NULL, "No memory map available!\n");

	struct stivale2_struct_tag_kernel_base_address *base_addr_info;
	base_addr_info = get_stivale_struct(stivale2_info, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
	assert(base_addr_info != NULL, "No base address info available!\n");

	struct stivale2_struct_tag_modules *modules;
	modules = get_stivale_struct(stivale2_info, STIVALE2_STRUCT_TAG_MODULES_ID);
	assert(modules != NULL, "No kernel module info available!\n");

	struct stivale2_struct_tag_rsdp *rsdp_tag;
	rsdp_tag = get_stivale_struct(stivale2_info, STIVALE2_STRUCT_TAG_RSDP_ID);
	assert(rsdp_tag != NULL, "No rsdp table pointer found!\n");

	boot_info.fb_print = write_err_ptr;

	boot_info.stack_addr = (void*) os_stack + sizeof(os_stack);

	boot_info.rsdp = (void*) rsdp_tag->rsdp;

	boot_info.fb_info.fb_addr	= (void*) fb_info->framebuffer_addr;
	boot_info.fb_info.fb_width	= fb_info->framebuffer_width;
	boot_info.fb_info.fb_height	= fb_info->framebuffer_height;
	boot_info.fb_info.fb_pitch	= fb_info->framebuffer_pitch;

	boot_info.mem_info.mem_entries = mem_info->entries;
	boot_info.mem_info.mem_map	   = (struct mem_entry*) mem_info->memmap;
	boot_info.mem_info.mem_pmm_base = (u64) base_addr_info->physical_base_address;
	boot_info.mem_info.mem_vmm_base = (u64) base_addr_info->virtual_base_address;

	boot_info.module_info.module_count = modules->module_count;
	boot_info.module_info.modules      = (struct kernel_module*) modules->modules;

	kmain(&boot_info);

	halt:
	for (;;) {
		asm("hlt");
	}
}

