#include <process/process.h>
#include <process/elf.h>
#include <process/scheduler.h>

#include <boot/boot_info.h>
#include <memory/vmm.h>

#include <string.h>
#include <panic.h>
#include <io/stdio.h>


static struct module_info *_module_info;

#define MAX_PROCESSES (1000)
static struct process _processes[1000];
static u64 _current_process = 0;


extern void asm_jump_usermode(u64 addr, u64 stack, u64 prog, u64 size);

static struct kernel_module *search_module(const char *module) {
	u32 nmodules = _module_info->module_count;

	for (u32 i = 0; i < nmodules; i++) {
		struct kernel_module *m = _module_info->modules[i];
		if (strcmp(m->path, module) == 0) {
			return m;
		}
	}
	return NULL;
}

static u64 first_free_process(void) {
	for (u64 i = 1; i < MAX_PROCESSES; i++) {
		if (_processes[i].status == PROC_NONE) {
			return i;
		}
	}
	return 0;
}

void process_init(struct boot_info *boot_info) {
	_module_info = &boot_info->module_info;

	for (u64 i = 0; i < MAX_PROCESSES; i++) {
		_processes[i] = (struct process){0};
	}

	scheduler_init();
}

u64 process_create(const char *name) {
	struct kernel_module *km = search_module(name);
	if (km == NULL) {
		return 0;
	}

	u64 pid = first_free_process();
	if (pid == 0) {
		return 0;
	}

	int err = elf_validate(km->addr);
	if (err != ELF_OK) {
		panic("Could not validate %s: %s\n", name, elf_error_str(err));
	}

	u64 *pagedir;
	u64 entry;
	err = elf_load(km->addr, &pagedir, &entry);
	if (err != ELF_OK) {
		panic("Could not load %s: %s\n", name, elf_error_str(err));
	}

	vmm_map(pagedir, VMM_USER_STACK_END - VMM_USER_STACK_LEN, VMM_USER_STACK_LEN);
	vmm_set_pagedir(pagedir);

	_processes[pid].status  = PROC_RUNNING;
	_processes[pid].pagedir = pagedir;
	_current_process = pid;

	asm_jump_usermode(entry, VMM_USER_STACK_END, (u64) NULL, 0);
	return pid;
}

struct process *process_get(u64 pid) {
	if (pid == 0 || pid >= MAX_PROCESSES) {
		return NULL;
	}
	return &_processes[pid];
}

u64 process_current(void) {
	return _current_process;
}

