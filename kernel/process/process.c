#include <boot/boot_info.h>
#include <io/stdio.h>
#include <memory/vmm.h>
#include <panic.h>
#include <process/elf.h>
#include <process/pipe.h>
#include <process/process.h>
#include <process/scheduler.h>
#include <string.h>

#define MAX_PROCESSES (100)

static struct process _processes[MAX_PROCESSES];

static struct module_info *_module_info;

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

static pid_t first_free_process(void) {
	for (pid_t i = 0; i < MAX_PROCESSES; i++) {
		if (_processes[i].name == NULL) {
			return i;
		}
	}

	panic("No free processes left\n");
}

void process_init(struct boot_info *boot_info) {
	_module_info = &boot_info->module_info;

	for (pid_t i = 0; i < MAX_PROCESSES; i++) {
		_processes[i] = (struct process){0};
	}

	scheduler_init();

	kprintf("initialized process system\n");
}

static pid_t load_to_memory(const char *name, u64 **pagedir, u64 *entry) {
	struct kernel_module *km = search_module(name);
	if (km == NULL) {
		panic("Could not find kernel module %s\n", name);
	}

	pid_t pid = first_free_process();

	int err = elf_validate(km->addr);
	if (err != ELF_OK) {
		panic("Could not validate %s: %s\n", name, elf_error_str(err));
	}

	err = elf_load(km->addr, pagedir, entry);
	if (err != ELF_OK) {
		panic("Could not load %s: %s\n", name, elf_error_str(err));
	}

	vmm_map(*pagedir, (void *) (VMM_USER_STACK_END - VMM_USER_STACK_LEN), VMM_USER_STACK_LEN);

	return pid;
}

void process_start_init(const char *name) {
	pagedir_t *pagedir;
	u64 entry;
	pid_t pid = load_to_memory(name, &pagedir, &entry);
	if (pid != 0) {
		panic("Init process already started!\n");
	}

	_processes[0].name    = name;
	_processes[0].pagedir = pagedir;
	_processes[0].entry   = entry;
}

pid_t process_create(const char *name) {
	pagedir_t *pagedir;
	u64 entry;

	if (_processes[0].name == NULL) {
		panic("Can't start %s, init process does not exist\n", name);
	}

	pid_t pid = load_to_memory(name, &pagedir, &entry);

	struct process *p = &_processes[pid];
	p->name           = name;
	p->pagedir        = pagedir;
	p->entry          = entry;
	p->read_pipe      = pipe_new();
	p->write_pipe     = pipe_new();

	return pid;
}

void process_destroy(pid_t pid) {
	if (pid < 0 || pid >= MAX_PROCESSES) {
		return;
	}

	struct process *p = &_processes[pid];

	p->name = NULL;

	if (p->read_pipe.dev.close != NULL) {
		p->read_pipe.dev.close((struct io_device *) &p->read_pipe);
	}
	if (p->write_pipe.dev.close != NULL) {
		p->write_pipe.dev.close((struct io_device *) &p->write_pipe);
	}

	vmm_set_pagedir(NULL);
	vmm_pagedir_destroy(p->pagedir);
	p->entry = 0;
}

struct process *process_get(pid_t pid) {
	if (pid < 0 || pid >= MAX_PROCESSES) {
		return NULL;
	}

	return &_processes[pid];
}
