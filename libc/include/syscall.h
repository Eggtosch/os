#pragma once

#include <common.h>

/* syscall 0x0:
 * @params
 * rax = syscall number
 * rbx = name
 * rcx = flags
 * @return
 * rax = file descriptor, negative if invalid
 */
int sys_open(const char *name, u64 flags);

/* syscall 0x1:
 * @params
 * rax = syscall number
 * rbx = fd
 * rcx = buffer
 * rdx = count
 * @return
 * rax = number of bytes read
 */
u64 sys_read(int fd, void *buffer, u64 count);

/* syscall 0x2:
 * @params
 * rax = syscall number
 * rbx = fd
 * rcx = buffer
 * rdx = count
 * @return
 * rax = number of bytes written
 */
u64 sys_write(int fd, void *buffer, u64 count);

/* syscall 0x3
 * @params
 * rax = syscall number
 * rbx = addr (start addr of new mapping)
 * rcx = length (length of new mapping in bytes)
 * rdx = fd (optionally map data from fd, -1 if none)
 * rdi = flags
 * @return
 * rax = indication of success, negative if error occured
 */
int sys_mmap(void *addr, u64 length, int fd, u64 flags);

/* syscall 0x4
 * @params
 * rax = syscall number
 * rbx = addr (addr of mapping)
 * rcx = length (length of mapping)
 * @return
 * rax = indication of success, negative if error occured
 */
int sys_munmap(void *addr, u64 length);

/*
 * syscall 0x5
 * @params
 * rax = syscall number
 * @return -
 */
void sys_exit(void);

u64 sys_fb_info(void);
i64 sys_fb_write(u16 x, u16 y, u16 width, u16 height, u32 *buffer);
