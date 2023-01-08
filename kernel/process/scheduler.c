#include <process/scheduler.h>
#include <interrupts/interrupts.h>
#include <acpi/hpet.h>
#include <io/stdio.h>

static void pit_irq(struct cpu_state *state) {
	printf("pit irq: %p!\n", state->rip);
	hpet_next_timeout(HPET_TIMER_PIT);
}

void scheduler_init(void) {
	interrupt_register(INT_PIT, pit_irq, INT_KERNEL);
}

void scheduler_enable(bool enable) {
	interrupt_enable(INT_PIT, enable);
}

