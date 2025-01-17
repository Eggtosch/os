#include <boot/boot_info.h>
#include <io/stdio.h>
#include <memory/pmm.h>
#include <string.h>
#include <time/hpet.h>

#define HPET_EN (1 << 0)
#define HPET_LEGACY_EN (1 << 1)
#define TIMERN_ENABLE (1 << 2)
#define TIMERN_PERIODIC (1 << 3)
#define TIMERN_PERIODIC_CAPABLE (1 << 4)

#define SECONDS (0)
#define MILLISECONDS (1)
#define MICROSECONDS (2)
#define NANOSECONDS (3)

#define NS_PER_SEC (1000000000UL)
#define FS_PER_NS (1000000UL)

#define HPET_CAPABILITIES (0x00)
#define HPET_CONFIG (0x10)
#define HPET_STATUS (0x20)
#define HPET_COUNTER (0xF0)
#define HPET_TIMERN_CONFIG(n) (0x100 + 0x20 * n)
#define HPET_TIMERN_COMPARATOR(n) (0x108 + 0x20 * n)
#define HPET_TIMERN_FSB(n) (0x110 + 0x20 * n)

static void *hpet = NULL;
static u64 precision_ns;

static u64 read_word(u64 offset) {
	return *(volatile u64 *) (hpet + offset);
}

static void write_word(u64 offset, u64 word) {
	*(volatile u64 *) (hpet + offset) = word;
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
	return time / (read_word(HPET_CAPABILITIES) >> 32);
}

static void init_timers(void) {
	for (int n = 0; n < 2; n++) {
		write_word(HPET_TIMERN_COMPARATOR(n), n == 0 ? 0 : time_to_comparator(1, SECONDS));
		u64 config = read_word(HPET_TIMERN_CONFIG(n));
		config |= TIMERN_ENABLE;
		write_word(HPET_TIMERN_CONFIG(n), config);
	}
}

void hpet_register(u64 addr) {
	kprintf("register time source: hpet\n");

	hpet = (struct hpet *) pmm_to_virt(addr);

	init_timers();

	write_word(HPET_COUNTER, 0);
	u64 config = read_word(HPET_CONFIG);
	config |= HPET_EN | HPET_LEGACY_EN;
	write_word(HPET_CONFIG, config);

	precision_ns = (read_word(HPET_CAPABILITIES) >> 32) / FS_PER_NS;

	int ntimers = (read_word(HPET_CAPABILITIES) >> 8) & 0x1f;
	kprintf("hpet: %d timers, %u ns precision\n", ntimers, precision_ns);
}

void hpet_delay_us(u64 us) {
	u64 end = read_word(HPET_COUNTER) + time_to_comparator(us, MICROSECONDS);
	while (read_word(HPET_COUNTER) <= end) {
		asm("pause");
	}
}

void hpet_delay_ms(u64 ms) {
	hpet_delay_us(ms * 1000);
}

void hpet_next_timeout(void) {
	u64 comparator = read_word(HPET_TIMERN_COMPARATOR(1));
	comparator += time_to_comparator(1, SECONDS);
	write_word(HPET_TIMERN_COMPARATOR(1), comparator);
}

u64 hpet_precision_ns(void) {
	return precision_ns;
}

u64 hpet_current_ns(void) {
	if (hpet == NULL) {
		return 0;
	}

	u64 next_ns              = read_word(HPET_TIMERN_COMPARATOR(HPET_TIMER_RTC)) * precision_ns;
	u64 current_ns           = read_word(HPET_COUNTER) * precision_ns;
	u64 ns_until_next_second = next_ns - current_ns;
	u64 time_since_last_sec  = NS_PER_SEC - ns_until_next_second;
	return time_since_last_sec;
}
