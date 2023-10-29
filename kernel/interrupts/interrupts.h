#pragma once

#include <common.h>

struct cpu_state {
	u64 r15;
	u64 r14;
	u64 r13;
	u64 r12;
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rsi;
	u64 rdi;
	u64 rbp;
	u64 rdx;
	u64 rcx;
	u64 rbx;
	u64 rax;

	u64 isr_number;
	u64 error_code;

	u64 rip;
	u64 cs;
	u64 eflags;
} __attribute__((packed));

typedef void (*isr_func)(struct cpu_state*);

#define INT_KERNEL (0)
#define INT_USER   (1)

#define INT_BASE (0x20)

#define INT_PIT        (0x20)
#define INT_KEYBOARD   (0x21)
#define INT_SLAVE_IRQS (0x22)
#define INT_RTC        (0x28)
#define INT_SYSCALL    (0x80)
#define INT_SCHED      (0x81)

void interrupt_register(u8 isr_num, isr_func f, u32 int_type);
void interrupt_enable(u8 isr_num, bool enable);
