// SPDX-License-Identifier: GPL-2.0
/*
 * string.c - Basic string/memory utilities
 */

#include <klib/string.h>

void memcpy(void *src, void *dest, int size)
{
	unsigned char *s = (unsigned char *)src;
	unsigned char *d = (unsigned char *)dest;
	int i;

	for (i = 0; i < size; i++)
		d[i] = s[i];
}

void memset(void *src, unsigned char val, int size) {
	unsigned char *s = (unsigned char *)src;
	int i;

	for (i = 0; i < size; i++)
		s[i] = val;
}

void strcpy(char *str1, char *str2)
{
	while ((*str1++ = *str2++));
}

int strcmp(char *str1, char *str2)
{
	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	return (unsigned char)*str1 - (unsigned char)*str2;
}
