#pragma once

#include <io/stdio.h>

void vfs_init(void);
int vfs_mount(const char *pathname, struct io_device *stream);
void vfs_unmount(int fd);
int vfs_open(const char *pathname);
struct io_device *vfs_get(int fd);
