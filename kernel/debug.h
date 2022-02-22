#pragma once

#include <common.h>

#define DEBUG_INFO   0
#define DEBUG_WARN   1
#define DEBUG_ERROR  2
#define DEBUG_SERIAL 3

void debug(u32 level, const char *fmt, ...);
