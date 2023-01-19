#pragma once

#include <common.h>

struct io_device;

typedef u64 (*io_dev_read)(struct io_device*, u8*, u64, u64);
typedef u64 (*io_dev_write)(struct io_device*, u8*, u64, u64);
typedef void (*io_dev_close)(struct io_device*);

struct io_device {
	io_dev_read read;
	io_dev_write write;
	io_dev_close close;
	void *userdata;
};

