#include <acpi/hpet.h>
#include <io/stdio.h>
#include <boot/boot_info.h>
#include <string.h>

#define HPET_EN					(1 << 0)
#define HPET_LEGACY_EN			(1 << 1)
#define TIMERN_ENABLE			(1 << 2)
#define TIMERN_PERIODIC			(1 << 3)
#define TIMERN_PERIODIC_CAPABLE (1 << 4)

#define SECONDS      (0)
#define MILLISECONDS (1)
#define MICROSECONDS (2)
#define NANOSECONDS  (3)

#define NS_PER_SEC (1000000000UL)
#define FS_PER_NS (1000000UL)

struct hpet {
	u64 capabilities;
	u64 reserved1;
	u64 config;
	u64 reserved3;
	u64 status;
	u64 reserved4[25];
	u64 counter;
	u64 reserved5;
	volatile struct {
		u64 config;
		u64 comparator;
		u64 fsb;
		u64 reserved0;
	} timers[];
};

static struct hpet *hpet = NULL;
static u64 precision_ns;

static u64 timern_timeout[2];
static bool timern_periodic_capable[2];

static u64 read_bitfield(u64 value, u64 start, u64 length) {
	return (value >> start) & ((u64) 0xffffffffffffffff >> (64 - length));
}

static u64 time_to_comparator(u64 time, u64 unit) {
	if (unit == SECONDS) {
		time *= 1000000000000000UL;
	} else if (unit == MILLISECONDS) {
		time *= 1000000000000UL;
	} else if (unit == MICROSECONDS) {
		time *= 1000000000UL;
	} else {
		time *= 1000000UL;
	}
	return time / read_bitfield(hpet->capabilities, 32, 32);
}

static void init_timers(void) {
	for (int n = 0; n < 2; n++) {
		timern_periodic_capable[n] = (hpet->timers[n].config & TIMERN_PERIODIC_CAPABLE) != 0;
		if (timern_periodic_capable[n]) {
			hpet->timers[n].config |= TIMERN_PERIODIC;
		}

		hpet->timers[n].comparator = timern_timeout[n];
		hpet->timers[n].config |= TIMERN_ENABLE;
	}
}

void hpet_register(u64 addr) {
	hpet = (struct hpet*) addr;

	timern_timeout[0] = time_to_comparator(250, MILLISECONDS);
	timern_timeout[1] = time_to_comparator(1, SECONDS);

	init_timers();

	hpet->counter = 0;
	hpet->config |= HPET_EN | HPET_LEGACY_EN;

	precision_ns = read_bitfield(hpet->capabilities, 32, 32) / FS_PER_NS;
}

void hpet_next_timeout(int timern) {
	if (timern < 0 || timern > 1) {
		return;
	}
	if (timern_periodic_capable[timern]) {
		return;
	}

	hpet->timers[timern].comparator += timern_timeout[timern];
}

u64 hpet_precision_ns(void) {
	return precision_ns;
}

u64 hpet_current_ns(void) {
	u64 next_ns = hpet->timers[HPET_TIMER_RTC].comparator / FS_PER_NS;
	u64 current_ns = hpet->counter / FS_PER_NS;
	u64 ns_until_next_second = next_ns - current_ns;
	u64 time_since_last_sec = NS_PER_SEC - ns_until_next_second;
	return time_since_last_sec;
}

