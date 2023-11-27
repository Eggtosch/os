#include <interrupts/interrupts.h>
#include <interrupts/lapic.h>
#include <io/stdio.h>
#include <memory/vmm.h>
#include <process/process.h>
#include <process/scheduler.h>

extern void asm_jump_usermode(u64 addr, u64 stack, u64 prog, u64 size);

static void sched_irq(__unused struct cpu_state *state) {
	kprintf("scheduler irq\n");
	/*pid_t pid = process_current();
	struct process *p = process_get(pid);
	if (p->status == PROC_NONE) {
	    return;
	} else if (p->status == PROC_RUNNABLE) {
	    p->status = PROC_RUNNING;
	    vmm_set_pagedir(p->pagedir);
	    asm_jump_usermode(p->entry, VMM_USER_STACK_END, (u64) NULL, 0);
	} else if (p->status == PROC_RUNNING) {
	    // replace with other process
	}*/
}

void scheduler_init(void) {
	interrupt_register(INT_SCHED, sched_irq, INT_KERNEL);
	kprintf("registered scheduler interrupt\n");
}

void scheduler_start(void) {
	apic_set_next_int(1);
}
