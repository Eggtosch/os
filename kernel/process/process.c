#include <process/process.h>

#include <memory/vmm.h>

#include <string.h>
#include <kexit.h>
#include <debug.h>
#include <common.h>


static struct module_info *_module_info;
static u8 *_osl_addr = NULL;
static u64 _osl_size = 0;

#define MAX_PROCESSES (1000)
static struct process _processes[1000];
static u64 _current_process = 0;


extern void asm_jump_usermode(u64 addr, u64 stack, u64 prog, u64 size);

static void search_module(const char *module, u8 **addr, u64 *size) {
	u32 nmodules = _module_info->module_count;

	for (u32 i = 0; i < nmodules; i++) {
		struct kernel_module *m = &_module_info->modules[i];
		if (strcmp(m->string, module) == 0) {
			*addr = m->begin;
			*size = m->end - m->begin;
			return;
		}
	}
	*addr = NULL;
	*size = 0;
}

static u64 first_free_process(void) {
	for (u64 i = 1; i < MAX_PROCESSES; i++) {
		if (_processes[i].status == PROC_NONE) {
			return i;
		}
	}
	return 0;
}

void process_init(struct module_info *module_info) {
	_module_info = module_info;

	for (u64 i = 0; i < MAX_PROCESSES; i++) {
		_processes[i] = (struct process){0};
	}

	search_module("osl", &_osl_addr, &_osl_size);

	if (_osl_addr == NULL) {
		debug(DEBUG_ERROR, "OSL module needed for processes was not found in loaded kernel modules!");
		kexit();
	} else {
		debug(DEBUG_INFO, "found osl binary: %p, %#x", _osl_addr, _osl_size);
	}
}

u64 process_create(const char *name) {
	char *prog;
	u64 prog_size;
	search_module(name, (u8**) &prog, &prog_size);
	if (prog == NULL) {
		debug(DEBUG_ERROR, "could not find program %s", name);
		return 0;
	}

	u64 pid = first_free_process();
	if (pid == 0) {
		debug(DEBUG_ERROR, "max number of processes reached: %d", MAX_PROCESSES - 1);
		return 0;
	}

	u64 *pagedir = vmm_pagedir_create();
	vmm_map_data(pagedir, VMM_USER_CODE, _osl_addr, _osl_size);
	u64 prog_addr = ALIGNUP_PAGE(VMM_USER_CODE + _osl_size);
	vmm_map_data(pagedir, prog_addr, prog, prog_size);
	vmm_map(pagedir, VMM_USER_STACK_END - VMM_USER_STACK_LEN, VMM_USER_STACK_LEN);
	vmm_set_pagedir(pagedir);
	_processes[pid].status  = PROC_RUNNING;
	_processes[pid].pagedir = pagedir;
	_current_process = pid;
	asm_jump_usermode(VMM_USER_CODE, VMM_USER_STACK_END, prog_addr, prog_size);
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
