#include <interrupts/interrupts.h>
#include <interrupts/lapic.h>
#include <io/stdio.h>
#include <memory/vmm.h>
#include <mutex.h>
#include <process/kernel_task.h>
#include <process/process.h>
#include <process/scheduler.h>

struct scheduler {
	int total;
	int ready;
	int blocked;
	int running;
	int last_proc;
	tid_t procs[MAX_CPUS];
};

extern void asm_jump_usermode(u64 addr, u64 stack, u64 prog, u64 size);

static mutex_t sched_mutex;
static struct scheduler scheduler;

static void sched_irq(struct cpu_state *state) {
	mutex_lock(&sched_mutex);

	apic_next_int_ms(1000);
	u8 cpu = apic_current_cpu();
	if (scheduler.ready == 0) {
		mutex_unlock(&sched_mutex);
		return;
	}

	tid_t old_task = scheduler.procs[cpu];
	tid_t new_task = kernel_task_get_next_rr(scheduler.last_proc);
	if (old_task == -1) {
		scheduler.ready -= 1;
		scheduler.running += 1;
	}

	scheduler.last_proc = new_task;
	kprintf("%d -> %d\n", old_task, new_task);
	kernel_task_swap_cpu_state(old_task, new_task, state);
	scheduler.procs[cpu] = new_task;

	mutex_unlock(&sched_mutex);
}

void scheduler_init(void) {
	mutex_init(&sched_mutex);

	scheduler = (struct scheduler){0};
	for (int i = 0; i < MAX_CPUS; i++) {
		scheduler.procs[i] = -1;
	}

	interrupt_register(INT_SCHED, sched_irq, INT_KERNEL);
	kprintf("registered scheduler interrupt\n");
}

void scheduler_add_task(void) {
	mutex_lock(&sched_mutex);
	scheduler.total += 1;
	scheduler.ready += 1;
	mutex_unlock(&sched_mutex);
}

void scheduler_start(void) {
	apic_next_int(1);
}
