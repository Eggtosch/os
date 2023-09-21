#pragma once

#include <common.h>

/* syscall 0x00: open a file.
 * name: file path.
 * flags: unused at the moment.
 * return: file descriptor, negative if invalid.
 */
int sys_open(const char *name, u64 flags);

/* syscall 0x01: read bytes from a file.
 * fd: file descriptor.
 * buffer: where to read into.
 * count: number of bytes to read.
 * return: number of bytes actually read.
 */
u64 sys_read(int fd, void *buffer, u64 count);

/* syscall 0x02: write bytes to a file.
 * fd: file descriptor.
 * buffer: data to write.
 * count: number of bytes to write.
 * return: number of bytes actually written.
 */
u64 sys_write(int fd, void *buffer, u64 count);

/* syscall 0x03: map memory.
 * addr: virtual address of new mapping.
 * length: length of new mapping in bytes.
 * fd: optionally map data from fd, -1 if none.
 * flags: unused at the moment.
 * return: status, negative if error occured.
 */
int sys_mmap(void *addr, u64 length, int fd, u64 flags);

/* syscall 0x04: unmap memory.
 * addr: virtual address of mapping.
 * length: length of mapping.
 * return: status, negative if error occured.
 */
int sys_munmap(void *addr, u64 length);

/*
 * syscall 0x05: exit the calling process.
 */
void sys_exit(void);

u64 sys_fb_info(void);
i64 sys_fb_write(u16 x, u16 y, u16 width, u16 height, u32 *buffer);
