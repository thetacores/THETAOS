#include <klib/string.h>

void memcpy(void *src, void *dest, int size) {
	unsigned char *s = (unsigned char *)src;
	unsigned char *d = (unsigned char *)dest;
	for (int i = 0; i < size; i++) {
		d[i] = s[i];
	}
}

void strcpy(char *str1, char *str2) {
	while ((*str1++ = *str2++));
}
