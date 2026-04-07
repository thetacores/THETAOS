// SPDX-License-Identifier: GPL-2.0
/*
 * kprintf.c - Kernel printf implementation
 *
 * Supports %d, %x, %s, %c format specifiers.
 */

#include <klib/kprintf.h>
#include <drivers/vga.h>
#include <klib/string.h>
#include <stdarg.h>

void kprintf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			case 'd': {
				char *d = itoa(va_arg(args, int));

				vga_write(d);
				break;
			}
			case 'c': {
				char c = (char)(va_arg(args, int));

				vga_putc(c);
				break;
			}
			case 's': {
				char *s = va_arg(args, char *);

				vga_write(s);
				break;
			}
			case 'x': {
				char *x = htoa(va_arg(args, unsigned int));

				vga_write(x);
				break;
			}
			}
		} else {
			vga_putc(*fmt);
		}
		fmt++;
	}
	va_end(args);
}
