#pragma once

#include <boot/boot_info.h>


#define PROC_INVALID (0)

#define PROC_NONE    (0)
#define PROC_RUNNING (1)

struct process {
	u64 status;
	u64 *pagedir;
};

void process_init(struct boot_info *boot_info);
u64  process_create(const char *name);
struct process *process_get(u64 pid);
u64  process_current(void);

