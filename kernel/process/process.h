#pragma once

#include <boot/boot_info.h>
#include <memory/vmm.h>
#include <process/pipe.h>

struct process {
	const char *name;
	pagedir_t *pagedir;
	u64 entry;
	struct pipe read_pipe;
	struct pipe write_pipe;
};

typedef i64 pid_t;

void process_init(struct boot_info *boot_info);
void process_start_init(const char *name);
pid_t process_create(const char *name);
void process_destroy(pid_t pid);

struct process *process_get(pid_t pid);
