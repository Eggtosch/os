#pragma once

#define panic(fmt, ...) _panic(__FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
__attribute__((noreturn)) void _panic(const char *file, const char *function, int line, const char *fmt, ...);
