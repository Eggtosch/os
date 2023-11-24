#pragma once

#include <common.h>

/*
 * get length of '\0' terminated string.
 * s: string.
 * return: length.
 */
u64 strlen(const char *s);

/*
 * compare two '\0' terminated strings.
 * s1: first string.
 * s2: second string.
 * return: negative if s1 is smaller, 0 if equal, positive else.
 */
int strcmp(const char *s1, const char *s2);

/*
 * copy one string to another location, unchecked.
 * dst: where to copy the string.
 * src: string to copy.
 * return: bytes that were copied.
 */
u64 strcpy(char *dst, const char *src);

/*
 * copy one string to another location, but at most n bytes.
 * dst: where to copy the string.
 * src: string to copy.
 * dst_len: copy at most this amount of bytes.
 * return: the bytes copied.
 */
u64 strncpy_s(char *dst, const char *src, u64 dst_len);

/*
 * set memory to value.
 * ptr: memory location to set.
 * value: value to use.
 * num: number of bytes to set.
 * return: ptr.
 */
void *memset(void *ptr, int value, u64 num);

/*
 * copy bytes from one memory location to another.
 * dst: location to copy to.
 * src: location to copy from.
 * count: number of bytes to copy.
 * return: dst.
 */
void *memcpy(void *dst, const void *src, u64 count);

/*
 * compare bytes at two memory locations.
 * ptr1: first memory location.
 * ptr2: second memory location.
 * count: number of bytes to compare.
 * return: a negative number when the first unequal byte is smaller, 
 *    positive if it is bigger, 0 if all are equal.
 */
int memcmp(void *ptr1, void *ptr2, u64 count);
