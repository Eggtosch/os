#pragma once

#include <common.h>
#include <vfs/io_device.h>

extern struct io_device *stdio;

void stdio_init(struct io_device *_stdio);

u64  snprintf(char *buf, u64 maxlen, const char *fmt, ...);
u64 vsnprintf(char *buf, u64 maxlen, const char *fmt, va_list args);
u64   printf(const char *fmt, ...);
u64  vprintf(const char *fmt, va_list args);
u64  fprintf(struct io_device *stream, const char *fmt, ...);
u64 vfprintf(struct io_device *stream, const char *fmt, va_list args);
