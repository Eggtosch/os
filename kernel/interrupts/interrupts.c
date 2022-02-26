#include <interrupts/interrupts.h>
#include <debug.h>
#include <common.h>
#include <device/pic.h>


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


void interrupt_register(u32 isr_num, isr_func f) {
	if (isr_num >= 255) {
		return;
	}
	isr_functions[isr_num] = f;
}

u64 interrupt_handler(u64 rsp) {
	struct cpu_state *cpu_state = (struct cpu_state*) rsp;
	u32 isr_num = cpu_state->isr_number;

	if (isr_num < 32) {
		debug(DEBUG_INFO, "(kernel-panic) %d -> %s", isr_num, exception_names[isr_num]);
		if (isr_num == 0xe) {
			u64 cr2;
			asm volatile("mov %%cr2, %0" : "=r"(cr2) :: "memory");
			debug(DEBUG_INFO, "fault address: %p, error code: %#x", cr2, cpu_state->error_code);
		}
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
