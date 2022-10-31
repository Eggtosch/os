#include <interrupts/interrupts.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <io/stdio.h>

#include <panic.h>

static const char *exception_names[] = {
	"#DE: divide by zero",
	"#DB: debug",
	" - : non maskable interrupt",
	"#BP: breakpoint",
	"#OF: overflow",
	"#BR: bound range exceeded",
	"#UD: invalid opcode",
	"#NM: device not available",
	"#DF: double fault",
	" - : (reserved)",
	"#TS: invalid tss",
	"#NP: segment not present",
	"#SS: stack segment fault",
	"#GP: general protection fault",
	"#PF: page fault",
	" - : (reserved)",
	"#MF: x87 floating point exception",
	"#AC: alignement check",
	"#MC: machine check",
	"#XF: simd floating point exception",
	"#VE: virtualization exception",
	"#CP: control protection exception",
	" - : (reserved)",
	" - : (reserved)",
	" - : (reserved)",
	" - : (reserved)",
	" - : (reserved)",
	" - : (reserved)",
	"#HV: hypervisor injection exception",
	"#VC: vmm communication exception",
	"#SX: security exception",
	" - : (reserved)"
};


static isr_func isr_functions[256];


void interrupt_register(u32 isr_num, isr_func f, u32 int_type) {
	if (isr_num >= 255) {
		return;
	}
	isr_functions[isr_num] = f;
	if (int_type == INT_USER) {
		idt_create_descriptor(isr_num, 0xee);
	}
}

u64 interrupt_handler(u64 rsp) {
	struct cpu_state *cpu_state = (struct cpu_state*) rsp;
	u32 isr_num = cpu_state->isr_number;
	u32 error_code = cpu_state->error_code;

	if (isr_num < 32) {
		if (isr_num == 0xe) {
			u64 cr2;
			asm volatile("mov %%cr2, %0" : "=r"(cr2) :: "memory");
			printf("fault address: %p, error code: %#x\n", cr2, error_code);
		} else if (isr_num == 0xd) {
			u64 gs;
			asm volatile("mov %%gs, %0" : "=r"(gs) :: "memory");
			printf("gs: %#x\n", gs);
		}
		panic("irq %d -> %s(%#x)\nInstruction: %p\n", isr_num, exception_names[isr_num], error_code, cpu_state->rip);
		while (1) {
			asm volatile("cli");
			asm volatile("hlt");
		}
	}

	if (isr_functions[isr_num] != NULL) {
		isr_functions[isr_num](cpu_state);
	}

	pic_signal_eoi(isr_num);

	return rsp;
}

void interrupt_enable(u32 isr_num, bool enable) {
	if (enable) {
		pic_clear_mask(isr_num);
	} else {
		pic_set_mask(isr_num);
	}
}

