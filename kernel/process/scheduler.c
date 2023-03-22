#include <process/scheduler.h>
#include <process/process.h>
#include <interrupts/interrupts.h>
#include <memory/vmm.h>
#include <acpi/hpet.h>
#include <io/stdio.h>
#include <driver/util.h>

extern void asm_jump_usermode(u64 addr, u64 stack, u64 prog, u64 size);

static void pit_irq(__unused struct cpu_state *state) {
	hpet_next_timeout(HPET_TIMER_PIT);

	pid_t pid = process_current();
	struct process *p = process_get(pid);
	if (p->status == PROC_NONE) {
		return;
	} else if (p->status == PROC_RUNNABLE) {
		p->status = PROC_RUNNING;
		vmm_set_pagedir(p->pagedir);
		asm_jump_usermode(p->entry, VMM_USER_STACK_END, (u64) NULL, 0);
	} else if (p->status == PROC_RUNNING) {
		// replace with other process
	}
}

void scheduler_init(void) {
	interrupt_register(INT_PIT, pit_irq, INT_KERNEL);
	kprintf("registered scheduler interrupt\n");
}

void scheduler_enable(bool enable) {
	i64 secs = time_since_boot();
	u64 ns = time_current_ns();
	u64 missed_ints = 4 * secs + ns / 250000000;
	for (u64 i = 0; i < missed_ints; i++) {
		hpet_next_timeout(HPET_TIMER_PIT);
	}
	kprintf("catching up %d scheduler interrupts\n", missed_ints);

	interrupt_enable(INT_PIT, enable);
	kprintf("%sabling scheduler timer interrupts\n", enable ? "en" : "dis");
}
