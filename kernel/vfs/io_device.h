#pragma once

#include <common.h>

struct io_device {
	u64 (*read)(struct io_device*, u8*, u64, u64);
	u64 (*write)(struct io_device*, u8*, u64, u64);
	void *userdata;
};

