#include <string.h>

size_t strlen(const char *s) {
	if (s == NULL) {
		return 0;
	}

	size_t len = 0;
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
