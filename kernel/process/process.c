#include <process/process.h>
#include <process/elf.h>
#include <process/scheduler.h>
#include <process/pipe.h>

#include <boot/boot_info.h>
#include <memory/vmm.h>

#include <string.h>
#include <panic.h>
#include <io/stdio.h>

#define MAX_PROCESSES (1000)

static struct process _processes[MAX_PROCESSES];
static pid_t _current_process = 0;

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
		if (_processes[i].status == PROC_NONE) {
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

	vmm_map(*pagedir, VMM_USER_STACK_END - VMM_USER_STACK_LEN, VMM_USER_STACK_LEN);
	vmm_set_pagedir(*pagedir);

	return pid;
}

void process_start_init(const char *name) {
	u64 *pagedir;
	u64 entry;
	pid_t pid = load_to_memory(name, &pagedir, &entry);
	if (pid != 0) {
		panic("Init process already started!\n");
	}

	_processes[0].status  = PROC_RUNNABLE;
	_processes[0].pagedir = pagedir;
	_processes[0].entry   = entry;
}

pid_t process_create(const char *name) {
	u64 *pagedir;
	u64 entry;

	if (_processes[0].status == PROC_NONE) {
		panic("Can't start %s, init process does not exist\n", name);
	}

	pid_t pid = load_to_memory(name, &pagedir, &entry);

	_processes[pid].status  = PROC_RUNNABLE;
	_processes[pid].pagedir = pagedir;
	_processes[pid].entry   = entry;
	_processes[pid].read_pipe = pipe_new();
	_processes[pid].write_pipe = pipe_new();

	return pid;
}

struct process *process_get(pid_t pid) {
	return &_processes[pid];
}

pid_t process_current(void) {
	return _current_process;
}

static struct io_device *get_fd(pid_t pid, int fd) {
	if (_processes[pid].status == PROC_NONE || fd < 0 || fd > 10) {
		return NULL;
	}

	if (fd == 0) {
		return &_processes[pid].read_pipe;
	} else if (fd == 1) {
		return &_processes[pid].write_pipe;
	} else {
		return _processes[pid].fds[fd - 2];
	}
}

struct io_device *process_get_fd(pid_t pid, int fd) {
	if (pid >= MAX_PROCESSES) {
		return NULL;
	}

	if (pid != 0) {
		return get_fd(pid, fd);
	}

	if (fd > MAX_PROCESSES * 2 + 8) {
		return NULL;
	} else if (fd < 8) {
		return get_fd(0, fd);
	} else {
		pid = fd / 2;
		return get_fd(pid, fd & 1);
	}
}

