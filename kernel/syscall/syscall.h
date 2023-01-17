#pragma once

#include <boot/boot_info.h>

void syscall_init(struct boot_info *boot_info);

/**
 * System call table:
 * 0x00: i64 open(const char *name);
 * 0x01: i64 read(u64 fd, u8 *buf, u64 count, u64 offset);
 * 0x02: i64 write(u64 fd, u8 *buf, u64 count, u64 offset);
 * 0x03: i64 poll(u64 fds[], u64 nfds);
 *
 * 0x10: void *mmap(???);
 * 0x11: void munmap(???);
 * 0x12: // create shared memory segment
 *
 * 0x20: struct process exec(const char *file); // pid, read fd, write fd
 * 0x21: void exit(u64 code);
 *
 * 0x30: struct time clock_time(void);
 * 0x31: struct time clock_res(void);
 * 0x32: struct time sleep(struct time time);
 */

