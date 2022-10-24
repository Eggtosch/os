#pragma once

#include <vfs/io_device.h>
#include <common.h>

// driver/serial.c
struct io_device *serial_io_device_get(void);

// driver/rtc.c
struct time {
	u8 seconds;
	u8 minutes;
	u8 hours;
	u8 day;
	u8 month;
	u8 year;
	u8 century;
};

i64 time(void);
i64 time_from_struct(struct time time);

