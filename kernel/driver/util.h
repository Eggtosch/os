#pragma once

#include <vfs/io_device.h>
#include <common.h>

// driver/serial.c
struct io_device *serial_io_device_get(void);

// driver/rtc.c
u64 time(void);

