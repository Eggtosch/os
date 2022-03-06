#include <debug.h>
#include <common.h>
#include <io/stdio.h>
#include <device/serial.h>
#include <console/console.h>

static const char *debug_levels[] = {"[INFO] ", "[WARN] ", "[ERROR]", "[SINFO]"};
static const char *debug_colors[] = {"", "\033[33m", "\033[31m", ""};
static const char *reset_color = "\033[0m";
static const u32 debug_colors_fb[] = {COLOR_WHITE, COLOR_YELLOW, COLOR_RED};
static const u32 reset_color_fb = COLOR_WHITE;

void debug(u32 level, const char *fmt, ...) {
	va_list args;
	va_list args2;
	va_start(args, fmt);
	va_copy(args2, args);

	fprintf (serial_io, "%s%s ", debug_colors[level], debug_levels[level]);
	vfprintf(serial_io, fmt, args);
	fprintf (serial_io, "%s\n", reset_color);

	if (level != DEBUG_SERIAL) {
		console_set_fgcolor(console_io, debug_colors_fb[level]);
		fprintf (console_io, "%s ", debug_levels[level]);
		vfprintf(console_io, fmt, args2);
		fprintf (console_io, "\n");
		console_set_fgcolor(console_io, reset_color_fb);
	}

	va_end(args);
	va_end(args2);
}
