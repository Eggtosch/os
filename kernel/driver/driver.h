#pragma once

#include <boot/boot_info.h>
#include <vfs/io_device.h>

typedef struct {
	const char *name;
	void (*func)(struct boot_info *boot_info);
} driver_init_t;

#define driver_name(func) __driver_init_##func
#define driver_init(name, func)                                                                    \
	__attribute__((section(".driver_init"), used)) driver_init_t driver_name(func) = {name, func}

struct driver_file {
	const char *filepath;
	struct io_device stream;
};

void driver_register(const char *name, struct driver_file *files);
