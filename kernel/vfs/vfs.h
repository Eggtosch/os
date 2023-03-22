#pragma once

#include <vfs/io_device.h>

void vfs_init(void);

int vfs_mount(const char *pathname, struct io_device *stream);
void vfs_unmount(int fd);

int vfs_open(const char *pathname);
u64 vfs_read(const char *pathname, u8 *buf, u64 buflen, u64 offset);
u64 vfs_write(const char *pathname, u8 *buf, u64 buflen, u64 offset);
struct io_device *vfs_get(int fd);

void vfs_print_entries(void);
