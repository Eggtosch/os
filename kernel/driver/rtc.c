#include <driver/driver.h>
#include <driver/util.h>
#include <interrupts/interrupts.h>
#include <io/io.h>
#include <io/stdio.h>
#include <acpi/acpi.h>
#include <acpi/hpet.h>

#define SECONDS_IN_MINUTE   (60L)
#define SECONDS_IN_HOUR     (60L * SECONDS_IN_MINUTE)
#define SECONDS_IN_DAY      (24L * SECONDS_IN_HOUR)
#define SECONDS_IN_MONTH(m) ((i64) days_in_month[m] * SECONDS_IN_DAY)
#define SECONDS_IN_YEAR     (365L * SECONDS_IN_DAY)

i8 days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static struct time rtc_global_time;

i64 time_from_struct(struct time time) {
	i64 timestamp = 0;

	timestamp += time.seconds;
	timestamp += SECONDS_IN_MINUTE * time.minutes;
	timestamp += SECONDS_IN_HOUR * time.hours;
	timestamp += SECONDS_IN_DAY * (time.day - 1);
	timestamp += SECONDS_IN_YEAR * time.year;
	timestamp += SECONDS_IN_YEAR * abs(20L - time.century) * 100;

	for (int i = 1; i < time.month; i++) {
		timestamp += SECONDS_IN_MONTH(i);
	}

	timestamp += SECONDS_IN_DAY * (time.year / 4);
	timestamp += SECONDS_IN_DAY * (abs(20L - time.century) / 4 + 1);

	bool this_year_is_leap_year = time.year % 4 == 0;
	bool leap_day_is_not_done = (time.month == 1) || (time.month == 2 && time.day <= 28);
	if (this_year_is_leap_year && leap_day_is_not_done) {
		timestamp -= SECONDS_IN_DAY;
	}

	if (time.century < 20) {
		timestamp = -timestamp;
	}

	return timestamp;
}

i64 time(void) {
	return time_from_struct(rtc_global_time);
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
	u8 century_reg = acpi_get_century_register();
	u8 century  = century_reg ? read_register(century_reg) : 20;

	if (!(status_b & 0x04)) {
		bool hours_pm_mode = (hours & 0x80) != 0;
		hours &= 0x7f;
		seconds = ((seconds >> 4) * 10) + (seconds & 0x0f);
		minutes = ((minutes >> 4) * 10) + (minutes & 0x0f);
		hours   = ((hours   >> 4) * 10) + (hours   & 0x0f) + (hours_pm_mode ? 0x80 : 0);
		day     = ((day     >> 4) * 10) + (day     & 0x0f);
		month   = ((month   >> 4) * 10) + (month   & 0x0f);
		year    = ((year    >> 4) * 10) + (year    & 0x0f);
		century = ((century >> 4) * 10) + (century & 0x0f);
	}
	if (!(status_b & 0x02)) {
		if (hours & 0x80) {
			hours = (hours & 0x7f) + 12;
		}
	}

	return (struct time){seconds, minutes, hours, day, month, year, century};
}

static void rtc_irq(__unused struct cpu_state *cpu_state) {
	u8 status_c = read_register(0x0c);
	if (!(status_c & 0x10)) {
		return;
	}
	rtc_global_time = get_rtc_struct();
	hpet_next_timeout(HPET_TIMER_RTC);
}

static u64 rtc_read_time(__unused struct io_device *dev, u8 *buf, u64 buflen, __unused u64 offset) {
	u64 timestamp = time();
	return snprintf((char*) buf, buflen, "%d", timestamp);
}

static u64 rtc_read_date(__unused struct io_device *dev, u8 *buf, u64 buflen, __unused u64 offset) {
	struct time time = rtc_global_time;
	return snprintf((char*) buf, buflen, "%0.2d.%0.2d.%d %0.2d:%0.2d:%0.2d",
		time.day, time.month, time.century * 100 + time.year, time.hours, time.minutes, time.seconds);
}

static struct driver_file driver_files[] = {
	{"/dev/date", {rtc_read_date, NULL, NULL, NULL}},
	{"/dev/time", {rtc_read_time, NULL, NULL, NULL}},
	{NULL, {NULL, NULL, NULL, NULL}},
};

static void rtc_init(__unused struct boot_info *boot_info) {
	rtc_global_time = get_rtc_struct();

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

