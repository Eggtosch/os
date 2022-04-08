#pragma once

#include <io/stdio.h>


extern struct io_device *serial_io;

void serial_init(void);
void serial_init_vfs(void);
