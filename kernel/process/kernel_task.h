#pragma once

#include <interrupts/interrupts.h>
#include <memory/vmm.h>
#include <process/process.h>

typedef i64 tid_t;

struct kernel_task {
	void *kernel_stack_top;
	struct cpu_state state;
	pid_t userspace_process;
};

void kernel_task_swap_cpu_state(tid_t old_task, tid_t new_task, struct cpu_state *state);
void kernel_task_system_init(void);
tid_t kernel_task_create(const char *name, void (*f)(void));
tid_t kernel_task_create_with_process(pid_t process);
void kernel_task_destroy(tid_t task);
bool kernel_task_switch(tid_t new_task);
