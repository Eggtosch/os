#include <boot/boot_info.h>
#include <boot/limine.h>

#include <string.h>
#include <common.h>

static struct limine_kernel_address_request kerneladdr_req = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0,
	.response = NULL
};

static struct limine_memmap_request memmap_req = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
	.response = NULL
};

static struct limine_rsdp_request rsdp_req = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0,
	.response = NULL
};

static struct limine_terminal_request terminal_req = {
	.id = LIMINE_TERMINAL_REQUEST,
	.revision = 0,
	.response = NULL,
	.callback = NULL
};

static struct limine_framebuffer_request framebuffer_req = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
	.response = NULL,
};

static struct limine_module_request module_req = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0,
	.response = NULL
};

__attribute__((section(".limine_reqs"), used))
static void *request[] = {
	&kerneladdr_req,
	&memmap_req,
	&rsdp_req,
	&terminal_req,
	&framebuffer_req,
	&module_req,
	NULL
};


static void write(const char *string, u64 length) {
	struct limine_terminal *term = terminal_req.response->terminals[0];
	terminal_req.response->write(term, string, length);
}

#define ERROR_HEADER  "Error: assert failed in '"
#define ERROR_HEADER2 "': ("
#define ERROR_TAIL    ")\n"
#define assert(cond, msg) _assert(cond, #cond, __func__, msg)
static void _assert(bool cond, const char *cond_str, const char *func_str, const char *msg) {
	if (cond) {
		return;
	}

	write(ERROR_HEADER, sizeof(ERROR_HEADER));
	write(func_str, strlen(func_str));
	write(ERROR_HEADER2, sizeof(ERROR_HEADER2));
	write(cond_str, strlen(cond_str));
	write(ERROR_TAIL, sizeof(ERROR_TAIL));

	if (msg == NULL) {
		return;
	}

	write(msg, strlen(msg));

	for (;;) {
		asm("hlt");
	}
}


static struct boot_info boot_info;

struct boot_info *boot_info_get(void) {
	return &boot_info;
}

extern void kmain(struct boot_info *boot_info);

void _start(void) {
	void *stack_addr;
	asm volatile("mov %%rsp, %0" : "=r"(stack_addr));

	if (terminal_req.response == NULL) {
		goto halt;
	}
	if (terminal_req.response->terminal_count == 0) {
		goto halt;
	}

	struct limine_framebuffer_response *fb_info = framebuffer_req.response;
	assert(fb_info != NULL && fb_info->framebuffer_count > 0, "No framebuffer available!\n");
	struct limine_framebuffer *fb = fb_info->framebuffers[0];
	assert(fb->bpp  == 32, "Kernel does only support 32 bpp framebuffer!\n");
	assert(fb->red_mask_size    ==  8 &&
		   fb->green_mask_size  ==  8 &&
		   fb->blue_mask_size   ==  8, "Kernel does only support 8 bit color masks!\n");
	assert(fb->red_mask_shift   == 16 &&
		   fb->green_mask_shift ==  8 &&
		   fb->blue_mask_shift  ==  0, "Kernel does only support RGB color format!\n");

	struct limine_memmap_response *memmap = memmap_req.response;
	assert(memmap != NULL, "No memory map available!\n");

	struct limine_kernel_address_response *base_addr_info = kerneladdr_req.response;
	assert(base_addr_info != NULL, "No base address info available!\n");

	struct limine_module_response *module_info = module_req.response;
	assert(module_info != NULL, "No kernel module info available!\n");

	struct limine_rsdp_response *rsdp_info = rsdp_req.response;
	assert(rsdp_info != NULL, "No rsdp table pointer found!\n");

	boot_info.fb_print = write;
	boot_info.stack_addr = stack_addr;
	boot_info.rsdp = rsdp_info->address;

	boot_info.fb_info.fb_addr	= fb->address;
	boot_info.fb_info.fb_width	= fb->width;
	boot_info.fb_info.fb_height	= fb->height;
	boot_info.fb_info.fb_pitch	= fb->pitch;

	boot_info.mem_info.mem_entries = memmap->entry_count;
	boot_info.mem_info.mem_map	   = (struct mem_entry**) memmap->entries;
	boot_info.mem_info.mem_pmm_base = base_addr_info->physical_base;
	boot_info.mem_info.mem_vmm_base = base_addr_info->virtual_base;

	boot_info.module_info.module_count = module_info->module_count;
	boot_info.module_info.modules      = (struct kernel_module**) module_info->modules;

	kmain(&boot_info);

	halt:
	for (;;) {
		asm("hlt");
	}
}

