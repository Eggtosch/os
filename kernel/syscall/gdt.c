#include <syscall/gdt.h>

#include <common.h>


struct gdt_entry {
	u16 limit_0_15;
	u16 base_0_15;
	u8  base_16_23;
	u8  type;
	u8  limit_16_19_and_flags;
	u8  base_24_31;
} __attribute__((packed));

struct gdt_tss_entry {
	u16 size;
	u16 base_low;
	u8  base_mid;
	u8  flags1;
	u8  flags2;
	u8  base_high;
	u32 base_upper;
	u32 reserved0;
} __attribute__((packed));

struct gdt {
	struct gdt_entry null;
	struct gdt_entry code_kernel_16;
	struct gdt_entry data_kernel_16;
	struct gdt_entry code_kernel_32;
	struct gdt_entry data_kernel_32;
	struct gdt_entry code_kernel_64;
	struct gdt_entry data_kernel_64;
	struct gdt_entry code_user_64;
	struct gdt_entry data_user_64;
	struct gdt_tss_entry tss;
} __attribute__((packed));

struct tss {
	u32 reserved0;
	u64 rsp0;
	u64 rsp1;
	u64 rsp2;
	u64 reserved1;
	u64 ist1;
	u64 ist2;
	u64 ist3;
	u64 ist4;
	u64 ist5;
	u64 ist6;
	u64 ist7;
	u64 reserved2;
	u16 reserved3;
	u16 iopb_offset;
} __attribute__((packed));

struct gdt_ptr {
	u16 limit;
	u64 base;
} __attribute__((packed));

static_assert(sizeof(struct gdt_entry) == 8, "");
static_assert(sizeof(struct gdt) == 11 * sizeof(struct gdt_entry), "");
static_assert(sizeof(struct tss) == 104, "");


static struct gdt _gdt;
static struct tss _tss;


extern void asm_load_gdt_and_tss(struct gdt_ptr *gdt_ptr);

static void copy_old_gdt(void) {
	struct gdt_ptr old_gdt_ptr;
	asm volatile("sgdt %0" : "=m"(old_gdt_ptr) :: );
	
	u64 *old_gdt = (u64*) old_gdt_ptr.base;
	for (u64 i = 0; i < (old_gdt_ptr.limit + 1) / sizeof(struct gdt_entry); i++) {
		((u64*) &_gdt)[i] = old_gdt[i];
	}
}

static struct gdt_entry create_gdt_descriptor(u64 base, u16 limit, u8 type, u8 flags) {
	struct gdt_entry entry;
	entry.limit_0_15            = limit & 0xffff;
	entry.base_0_15             = base & 0xffff;
	entry.base_16_23            = (base >> 16) & 0xff;
	entry.type                  = type;
	entry.limit_16_19_and_flags = (flags << 4) | ((limit >> 16) & 0xf);
	entry.base_24_31            = (base >> 24) & 0xff;
	return entry;
}

static struct gdt_tss_entry create_tss_descriptor(u64 base, u16 size, u8 flags1, u8 flags2) {
	struct gdt_tss_entry entry;
	entry.size       = size;
	entry.base_low   = base & 0xffff;
	entry.base_mid   = (base >> 16) & 0xff;
	entry.base_high  = (base >> 24) & 0xff;
	entry.base_upper = base >> 32;
	entry.flags1     = flags1;
	entry.flags2     = flags2;
	entry.reserved0  = 0;
	return entry;
}

static void create_userspace_and_tss_entries(void) {
	_gdt.code_user_64 = create_gdt_descriptor(0, 0, 0xfa, 0xa);
	_gdt.data_user_64 = create_gdt_descriptor(0, 0, 0xf2, 0xc);
	_gdt.tss          = create_tss_descriptor((u64) &_tss, sizeof(_tss) - 1, 0x89, 0x0);
}

void gdt_init(void *kernel_stack) {
	copy_old_gdt();
	create_userspace_and_tss_entries();
	for (u32 i = 0; i < sizeof(_tss); i++) {
		*(u8*) &_tss = 0;
	}
	_tss.rsp0 = (u64) kernel_stack;

	struct gdt_ptr new_gdt_ptr = { .limit = sizeof(_gdt) - 1, .base = (u64) &_gdt };

	asm_load_gdt_and_tss(&new_gdt_ptr);
}

