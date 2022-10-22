#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <interrupts/interrupts.h>

struct idt_descriptor {
	u16 offset_0_15;
	u16 selector;
	u8  ist;
	u8  type_and_attributes;
	u16 offset_16_31;
	u32 offset_32_63;
	u32 reserved;
} __attribute__((packed));

static_assert(sizeof(struct idt_descriptor) == 16, "sizeof struct idt_descriptor must be 16 bytes!");

struct idt_ptr {
	u16 limit;
	u64 base;
} __attribute__((packed));

static_assert(sizeof(struct idt_ptr) == 10, "sizeof struct idt_ptr must be 10!");


extern void asm_load_idt(struct idt_ptr *idt_ptr);
extern u64  asm_isr_names[];


static struct idt_descriptor _idt[256];
static struct idt_ptr        _idt_ptr;


void idt_create_descriptor(u8 index, u8 type_and_attributes) {
	u64 offset = asm_isr_names[index];
	struct idt_descriptor *desc = &_idt[index];

	desc->offset_0_15         = offset & 0xffff;
	desc->selector            = 0x28;
	desc->ist                 = 0;
	desc->type_and_attributes = type_and_attributes;
	desc->offset_16_31        = (offset >> 16) & 0xffff;
	desc->offset_32_63        = (offset >> 32) & 0xffffffff;
	desc->reserved            = 0;
}

void idt_init(void) {
	for (u8 i = 0; i < 32; i++) {
		idt_create_descriptor(i, 0x8e);
	}

	pic_remap();
	interrupt_enable(INT_SLAVE_IRQS, true);

	for (u16 i = 32; i < 256; i++) {
		idt_create_descriptor((u8) i, 0x8e);
	}

	_idt_ptr.limit = sizeof(_idt) - 1;
	_idt_ptr.base  = (u64) &_idt;

	asm_load_idt(&_idt_ptr);
}

