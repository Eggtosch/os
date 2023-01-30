#pragma once

#include <boot/boot_info.h>
#include <vfs/io_device.h>

#define PROC_MAX_FDS  (8)
#define PROC_NONE     (0)
#define PROC_RUNNING  (1)
#define PROC_RUNNABLE (2)

struct process {
	u64 status;
	u64 *pagedir;
	u64 entry;
	struct io_device read_pipe;
	struct io_device write_pipe;
	struct io_device *fds[8];
};

typedef u64 pid_t;

void process_init(struct boot_info *boot_info);
void process_start_init(const char *name);
pid_t process_create(const char *name);

struct process *process_get(pid_t pid);
pid_t process_current(void);
struct io_device *process_get_fd(pid_t pid, int fd);

