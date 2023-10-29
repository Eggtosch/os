#pragma once

#include <common.h>

#define HPET_TIMER_PIT (0)
#define HPET_TIMER_RTC (1)

/*
 * register a hpet device found in acpi tables.
 * addr: the addr from the acpi tables.
 */
void hpet_register(u64 addr);

/*
 * activate the next timeout for the rtc interrupt.
 */
void hpet_next_timeout(void);

/*
 * busy wait for n milliseconds.
 * ms: milliseconds.
 */
void hpet_delay_ms(u64 ms);

/*
 * busy wait for n microseconds.
 * us: microseconds.
 */
void hpet_delay_us(u64 us);

/*
 * get the precision of the hpet device in nanoseconds.
 * return: precision in nanoseconds.
 */
u64 hpet_precision_ns(void);

/*
 * get the current nanoseconds since the last full second.
 * return: current nanosenconds.
 */
u64 hpet_current_ns(void);
