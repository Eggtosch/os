#include <memory/pmm.h>
#include <panic.h>
#include <process/kernel_task.h>
#include <process/scheduler.h>
#include <string.h>

#define MAX_KERNEL_TASKS 200

static struct kernel_task _kernel_tasks[MAX_KERNEL_TASKS];

static tid_t first_free_task(void) {
	for (int i = 0; i < MAX_KERNEL_TASKS; i++) {
		if (_kernel_tasks[i].kernel_stack_top == NULL) {
			return i;
		}
	}

	return -1;
}

static struct kernel_task *get_task(tid_t task) {
	if (task < 0 || task >= MAX_KERNEL_TASKS) {
		return NULL;
	}

	if (_kernel_tasks[task].kernel_stack_top == NULL) {
		return NULL;
	}

	return &_kernel_tasks[task];
}

tid_t kernel_task_get_next_rr(tid_t t) {
	for (int i = 1; i <= MAX_KERNEL_TASKS; i++) {
		tid_t task = (t + i) % MAX_KERNEL_TASKS;
		if (get_task(task) != NULL) {
			return task;
		}
	}

	return 0;
}

void kernel_task_swap_cpu_state(tid_t old_task, tid_t new_task, struct cpu_state *state) {
	struct kernel_task *ktask_o = get_task(old_task);
	struct kernel_task *ktask_n = get_task(new_task);
	if (state == NULL) {
		return;
	}

	if (ktask_o != NULL) {
		memcpy(&ktask_o->state, state, sizeof(struct cpu_state));
	}

	if (ktask_n != NULL) {
		memcpy(state, &ktask_n->state, sizeof(struct cpu_state));
	}
}

void kernel_task_system_init(void) {
	for (int i = 0; i < MAX_KERNEL_TASKS; i++) {
		_kernel_tasks[i].kernel_stack_top = NULL;
		memset(&_kernel_tasks[i].state, 0, sizeof(struct cpu_state));
		_kernel_tasks[i].userspace_process = -1;
	}
}

tid_t kernel_task_create(const char *name, void (*f)(void)) {
	tid_t task = first_free_task();
	if (task < 0) {
		panic("No free kernel tasks!");
	}

	struct kernel_task *ktask = &_kernel_tasks[task];
	ktask->name               = name;
	ktask->kernel_stack_top   = pmm_alloc(KERNEL_TASK_STACK_PAGES);
	memset(&ktask->state, 0, sizeof(struct cpu_state));
	kprintf("new task: %x\n", sizeof(struct cpu_state));

	ktask->state.rip         = (u64) (void *) f;
	ktask->state.rbp         = (u64) ktask->kernel_stack_top;
	ktask->state.cs          = 0x28;
	ktask->state.eflags      = 1 << 9; // enable interrupts
	ktask->userspace_process = -1;
	scheduler_add_task();
	return task;
}

tid_t kernel_task_create_with_process(pid_t process) {
	tid_t task = first_free_task();
	if (task < 0) {
		panic("No free kernel tasks!");
	}

	struct process *proc = process_get(process);
	if (proc == NULL) {
		return -ESRCH;
	}

	struct kernel_task *ktask = &_kernel_tasks[task];
	ktask->name               = proc->name;
	ktask->kernel_stack_top   = pmm_alloc(KERNEL_TASK_STACK_PAGES);
	memset(&ktask->state, 0, sizeof(struct cpu_state));
	ktask->userspace_process = process;
	return task;
}

void kernel_task_destroy(tid_t task) {
	struct kernel_task *ktask = get_task(task);
	if (ktask == NULL) {
		return;
	}

	pmm_free(ktask->kernel_stack_top, KERNEL_TASK_STACK_PAGES);
	ktask->kernel_stack_top = NULL;

	memset(&ktask->state, 0, sizeof(struct cpu_state));

	process_destroy(ktask->userspace_process);
	ktask->userspace_process = -1;
}

bool kernel_task_switch(tid_t new_task) {
	return new_task == 0; // TODO
}
