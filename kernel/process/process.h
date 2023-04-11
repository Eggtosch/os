#pragma once

#include <boot/boot_info.h>
#include <memory/vmm.h>
#include <process/pipe.h>

#define PROC_NONE     (0)
#define PROC_RUNNING  (1)
#define PROC_RUNNABLE (2)

struct process {
	u64 status;
	pagedir_t *pagedir;
	u64 entry;
	struct pipe read_pipe;
	struct pipe write_pipe;
};

// u64
typedef u64 pid_t;

void process_init(struct boot_info *boot_info);
void process_start_init(const char *name);
pid_t process_create(const char *name);
void process_destroy(pid_t pid);

struct process *process_get(pid_t pid);
pid_t process_current(void);
