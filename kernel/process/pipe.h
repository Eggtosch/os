#pragma once

#include <vfs/io_device.h>

struct pipe {
	struct io_device dev;
	int position;
	int size;
	u8 *buffer;
};

struct pipe pipe_new(void);
