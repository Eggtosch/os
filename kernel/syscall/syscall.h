#pragma once

#include <boot/boot_info.h>

void syscall_init(struct boot_info *boot_info);

/**
 * System call table:
 * u64 open(const char *name);
 * u64 read(u64 fd, u8 *buf, u64 count, u64 offset);
 * u64 write(u64 fd, u8 *buf, u64 count, u64 offset);
 * void *mmap(???);
 * void munmap(???);
 * void exit(u64 code);
 * struct process exec(const char *file); // pid, read fd, write fd
 * struct time clock_time(void);
 * struct time clock_res(void);
 * ??? memshared_create(???);
 * struct time sleep(struct time time);
 * struct fb fb_get(void);
 * ??? fb_write(???);
 * ??? fb_read(???);
 */

/*
system calls:
- open (open a file and get a process specific file descriptor)
- read (read from a file)
- write (write to a file)
- poll (wait for a ready file)
- mmap/sbrk (allocate new memory)
- munmap/unsbrk
- exit (exit process)
- exec (spawn a new process from an elf file, return process structure with pid and fds)
- clock_time (get current time, seconds and nanoseconds)
- clock_res (get resolution of time)
- memshared_create (create named memory segment that can be shared between processes)
- sleep
- fb_get
- fb_write
- fb_read
-----------------
- timer_create/delete/gettime/settime
*/
