#pragma once

#include <common.h>

u64   strlen(const char *s);
int   strcmp(const char *s1, const char *s2);
u64   strcpy(char *dst, const char *src);
u64   strncpy_s(char *dst, const char *src, u64 dst_len);
void *memset(void *ptr, int value, u64 num);
void *memcpy(void *dst, const void *src, u64 count);
int   memcmp(void *ptr1, void *ptr2, u64 count);

