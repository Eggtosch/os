#include <interrupts/interrupts.h>
#include <interrupts/idt.h>
#include <interrupts/lapic.h>
#include <interrupts/ioapic.h>
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

void interrupt_register(u8 isr_num, isr_func f, u32 int_type) {
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
		char buf[200];
		if (isr_num == 0xe) {
			u64 cr2;
			asm volatile("mov %%cr2, %0" : "=r"(cr2) :: "memory");
			snprintf(buf, sizeof(buf), "fault address: %p, error code: %#x", cr2, error_code);
		} else if (isr_num == 0xd) {
			u64 gs;
			asm volatile("mov %%gs, %0" : "=r"(gs) :: "memory");
			snprintf(buf, sizeof(buf), "gs: %#x", gs);
		}
		const char *exc_name = exception_names[isr_num];
		u8 cpu = apic_current_cpu();
		panic("[cpu %d] %s\nirq %d -> %s(%#x)\nInstruction: %p", cpu, buf, isr_num, exc_name, error_code, cpu_state->rip);
	}

	if (isr_functions[isr_num] != NULL) {
		isr_functions[isr_num](cpu_state);
	}

	apic_signal_eoi();

	return rsp;
}

void interrupt_enable(u8 isr_num, bool enable) {
	ioapic_set_mask(isr_num, enable ? 0 : 1);
}
