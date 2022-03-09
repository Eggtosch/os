#pragma once

#include <common.h>


struct io_device {
	u64 (*read)(struct io_device*, u8*, u64);
	u64 (*write)(struct io_device*, u8*, u64);
};

extern struct io_device *stdio;

void stdio_init(struct io_device *_stdio);

void  snprintf(char *buf, u64 maxlen, const char *fmt, ...);
void vsnprintf(char *buf, u64 maxlen, const char *fmt, va_list args);
void   printf(const char *fmt, ...);
void  vprintf(const char *fmt, va_list args);
void  fprintf(struct io_device *stream, const char *fmt, ...);
void vfprintf(struct io_device *stream, const char *fmt, va_list args);
