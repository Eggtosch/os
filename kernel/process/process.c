#include <process/process.h>
#include <debug.h>
#include <string.h>
#include <common.h>
#include <kexit.h>


u8 *_osl_addr = NULL;
u64 _osl_size = 0;


void process_init(struct module_info *module_info) {
	u32 nmodules = module_info->module_count;

	for (u32 i = 0; i < nmodules; i++) {
		struct kernel_module *m = &module_info->modules[i];
		if (strcmp(m->string, "osl") == 0) {
			_osl_addr = m->begin;
			_osl_size = m->end - m->begin;
		}
	}

	if (_osl_addr == NULL) {
		debug(DEBUG_ERROR, "OSL module needed for processes was not found in loaded kernel modules!");
		kexit();
	}
}
