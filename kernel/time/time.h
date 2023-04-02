#pragma once

#include <common.h>

struct time {
	u8 seconds;
	u8 minutes;
	u8 hours;
	u8 day;
	u8 month;
	u8 year;
	u8 century;
};

void time_init(void);
i64 time(void);
i64 time_since_boot(void);
u64 time_current_ns(void);
i64 time_from_struct(struct time time);
