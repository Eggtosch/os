#include <driver/driver.h>
#include <interrupts/interrupts.h>
#include <io/io.h>
#include <io/stdio.h>

#define SECONDS_IN_MINUTE   (60UL)
#define SECONDS_IN_HOUR     (60UL * SECONDS_IN_MINUTE)
#define SECONDS_IN_DAY      (24UL * SECONDS_IN_HOUR)
#define SECONDS_IN_MONTH(m) ((u64) days_in_month[m] * SECONDS_IN_DAY)
#define SECONDS_IN_YEAR     (365UL * SECONDS_IN_DAY)

u8 days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct time {
	u8 seconds;
	u8 minutes;
	u8 hours;
	u8 day;
	u8 month;
	u8 year;
};

static struct time rtc_global_time;

u64 time(void) {
	struct time time = rtc_global_time;
	u64 timestamp = 0;

	timestamp += time.seconds;
	timestamp += SECONDS_IN_MINUTE * time.minutes;
	timestamp += SECONDS_IN_HOUR * time.hours;
	timestamp += SECONDS_IN_DAY * (time.day - 1);
	timestamp += SECONDS_IN_YEAR * time.year;

	for (int i = 1; i < time.month; i++) {
		timestamp += SECONDS_IN_MONTH(i);
	}

	timestamp += SECONDS_IN_DAY * (time.year / 4 + 1);

	bool this_year_is_leap_year = time.year % 4 == 0;
	bool leap_day_is_done = (time.month == 1) || (time.month == 2 && time.day <= 28);
	if (this_year_is_leap_year && !leap_day_is_done) {
		timestamp -= SECONDS_IN_DAY;
	}

	return timestamp;
}

static u8 read_register(u8 reg) {
	return io_outinb(0x70, reg, 0x71);
}

static void write_register(u8 reg, u8 value) {
	io_outb(0x70, reg);
	io_outb(0x71, value);
}

static struct time get_rtc_struct(void) {
	u8 status_b = read_register(0x0b);
	u8 seconds  = read_register(0x00);
	u8 minutes  = read_register(0x02);
	u8 hours    = read_register(0x04);
	u8 day      = read_register(0x07);
	u8 month    = read_register(0x08);
	u8 year     = read_register(0x09);

	if (!(status_b & 0x04)) {
		bool hours_pm_mode = (hours & 0x80) != 0;
		hours &= 0x7f;
		seconds = ((seconds >> 4) * 10) + (seconds & 0x0f);
		minutes = ((minutes >> 4) * 10) + (minutes & 0x0f);
		hours   = ((hours   >> 4) * 10) + (hours   & 0x0f) + (hours_pm_mode ? 0x80 : 0);
		day     = ((day     >> 4) * 10) + (day     & 0x0f);
		month   = ((month   >> 4) * 10) + (month   & 0x0f);
		year    = ((year    >> 4) * 10) + (year    & 0x0f);
	}
	if (!(status_b & 0x02)) {
		if (hours & 0x80) {
			hours = (hours & 0x7f) + 12;
		}
	}

	return (struct time){seconds, minutes, hours, day, month, year};
}

static void rtc_irq(struct cpu_state *cpu_state) {
	(void) cpu_state;
	u8 status_c = read_register(0x0c);
	if (!(status_c & 0x10)) {
		return;
	}
	rtc_global_time = get_rtc_struct();
}

static u64 rtc_read_time(struct io_device *dev, u8 *buf, u64 buflen) {
	(void) dev;
	u64 timestamp = time();
	return snprintf((char*) buf, buflen, "%d", timestamp);
}

static u64 rtc_read_date(struct io_device *dev, u8 *buf, u64 buflen) {
	(void) dev;
	struct time time = rtc_global_time;
	return snprintf((char*) buf, buflen, "%d.%d.%d %0.2d:%0.2d:%0.2d",
		time.day, time.month, 2000 + time.year, time.hours, time.minutes, time.seconds);
}

static struct driver_file driver_files[] = {
	{"/dev/date", {rtc_read_date, NULL, NULL}},
	{"/dev/time", {rtc_read_time, NULL, NULL}},
	{NULL, {NULL, NULL, NULL}},
};

static void rtc_init(struct boot_info *boot_info) {
	(void) boot_info;
	interrupt_register(INT_RTC, rtc_irq, INT_KERNEL);

	asm volatile("cli");
	u8 status_b = read_register(0x8b);
	status_b |= 0x10;
	status_b &= ~0x20;
	status_b &= ~0x40;
	write_register(0x8b, status_b);
	read_register(0x0c);

	interrupt_enable(INT_RTC, true);
	driver_register("rtc", driver_files);
	asm volatile("sti");
}

driver_init("rtc", rtc_init);

