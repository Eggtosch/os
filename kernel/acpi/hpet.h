#pragma once

#include <common.h>

#define HPET_TIMER_PIT (0)
#define HPET_TIMER_RTC (1)

void hpet_register(u64 addr);
void hpet_next_timeout(int timern);
u64 hpet_precision_ns(void);
u64 hpet_current_ns(void);
