#include <process/process.h>
#include <debug.h>
#include <string.h>
#include <common.h>
#include <kexit.h>
#include <memory/vmm.h>


u8 *_osl_addr = NULL;
u64 _osl_size = 0;


extern void asm_jump_usermode(u64 addr);

void process_init(struct module_info *module_info) {
	u32 nmodules = module_info->module_count;

	for (u32 i = 0; i < nmodules; i++) {
		struct kernel_module *m = &module_info->modules[i];
		if (strcmp(m->string, "osl") == 0) {
			_osl_addr = m->begin;
			_osl_size = m->end - m->begin;
			debug(DEBUG_INFO, "found osl binary: %p", _osl_addr);
		}
	}

	if (_osl_addr == NULL) {
		debug(DEBUG_ERROR, "OSL module needed for processes was not found in loaded kernel modules!");
		kexit();
	}
}

void process_start(const char *name) {
	(void) name;
	u64 *pagedir = vmm_pagedir_create();
	vmm_map_data(pagedir, 0x1000000000, _osl_addr, _osl_size);
	vmm_set_pagedir(pagedir);
	asm_jump_usermode(0x1000000000);
}
