#pragma once

#include <vfs/io_device.h>

// driver/serial.c
struct io_device *serial_io_device_get(void);
