#include <process/process.h>
#include <debug.h>
#include <string.h>
#include <common.h>
#include <kexit.h>
#include <memory/vmm.h>


struct module_info *_module_info;
u8 *_osl_addr = NULL;
u64 _osl_size = 0;


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

void process_init(struct module_info *module_info) {
	_module_info = module_info;

	search_module("osl", &_osl_addr, &_osl_size);

	if (_osl_addr == NULL) {
		debug(DEBUG_ERROR, "OSL module needed for processes was not found in loaded kernel modules!");
		kexit();
	} else {
		debug(DEBUG_INFO, "found osl binary: %p", _osl_addr);
	}
}

void process_start(const char *name) {
	char *prog;
	u64 prog_size;
	search_module(name, (u8**) &prog, &prog_size);
	if (prog == NULL) {
		debug(DEBUG_ERROR, "could not find program %s", name);
		return;
	}

	u64 *pagedir = vmm_pagedir_create();
	vmm_map_data(pagedir, VMM_USER_CODE, _osl_addr, _osl_size);
	u64 prog_addr = VMM_USER_CODE +
					_osl_size / PAGE_SIZE +
					(_osl_size % PAGE_SIZE == 0 ? 0 : PAGE_SIZE);
	vmm_map_data(pagedir, prog_addr, prog, prog_size);
	vmm_map(pagedir, VMM_USER_STACK_END - VMM_USER_STACK_LEN, VMM_USER_STACK_LEN);
	vmm_set_pagedir(pagedir);
	asm_jump_usermode(VMM_USER_CODE, VMM_USER_STACK_END, prog_addr, prog_size);
}
