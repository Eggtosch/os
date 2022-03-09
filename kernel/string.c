#include <string.h>

#include <common.h>


u64 strlen(const char *s) {
	if (s == NULL) {
		return 0;
	}

	u64 len = 0;
	while (*s) {
		len++;
		s++;
	}
	return len;
}

int strcmp(const char *s1, const char *s2) {
	if (s1 == NULL || s2 == NULL) {
		return -1;
	}
	while (*s1 == *s2 && *s1) {
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

void * memset(void *ptr, int value, u64 num) {
	u8 *p = (u8*) ptr;
	for (u64 i = 0; i < num; i++) {
		p[i] = value;
	}
	return ptr;
}

void * memcpy(void *dst, const void *src, u64 count) {
	u8 *dst0 = (u8*) dst;
	u8 *src0 = (u8*) src;
	for (u64 i = 0; i < count; i++) {
		dst0[i] = src0[i];
	}
	return dst;
}
