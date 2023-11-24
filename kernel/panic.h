#pragma once

/*
 * print message to serial output and frame buffer and halt.
 * fmt: format string.
 */
__attribute__((noreturn)) void panic(const char *fmt, ...);
