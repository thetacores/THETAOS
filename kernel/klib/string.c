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

char *itoa(int num)
{
	static char buf[12];
	int i = 0;
	int is_negative = 0;

	if (num == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}

	if (num < 0) {
		is_negative = 1;
		num = -num;
	}

	while (num > 0) {
		buf[i++] = '0' + (num % 10);
		num /= 10;
	}

	if (is_negative)
		buf[i++] = '-';

	buf[i] = '\0';

	/* reverse */
	int left = 0, right = i - 1;

	while (left < right) {
		char tmp = buf[left];

		buf[left++] = buf[right];
		buf[right--] = tmp;
	}

	return buf;
}

char *htoa(unsigned int num)
{
	static char buf[11];
	const char hex_chars[] = "0123456789ABCDEF";
	int i = 0;

	if (num == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}

	while (num > 0) {
		buf[i++] = hex_chars[num & 0xF];
		num >>= 4;
	}

	buf[i] = '\0';

	/* reverse */
	int left = 0, right = i - 1;

	while (left < right) {
		char tmp = buf[left];

		buf[left++] = buf[right];
		buf[right--] = tmp;
	}

	return buf;
}

int atoi(char *str)
{
	int res = 0;
	int sign = 1;
	int i = 0;

	if (str[0] == '-') {
		sign = -1;
		i++;
	}

	while (str[i] != '\0') {
		if (str[i] >= '0' && str[i] <= '9') {
			res = res * 10 + (str[i] - '0');
		} else {
			break; 
		}
		i++;
	}

	return sign * res;
}
