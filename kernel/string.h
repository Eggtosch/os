#pragma once

#include <common.h>


u64   strlen(const char *s);
int   strcmp(const char *s1, const char *s2);
void *memset(void *ptr, int value, u64 num);
void *memcpy(void *dst, const void *src, u64 count);
