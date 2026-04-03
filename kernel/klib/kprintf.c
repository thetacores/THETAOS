#include <klib/kprintf.h>
#include <drivers/vga.h>
#include <stdarg.h>

char *itoa(int num) {
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

	int left = 0, right = i - 1;
	while (left < right) {
		char tmp = buf[left];
		buf[left++] = buf[right];
		buf[right--] = tmp;
	}

	return buf;
}

char *htoa(unsigned int num) {
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

	int left = 0, right = i - 1;
	while (left < right) {
		char tmp = buf[left];
		buf[left++] = buf[right];
		buf[right--] = tmp;
	}

	return buf;
}

void kprintf(const char *fmt, ...) {
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
						  char *s = va_arg(args, char*);
						  vga_write(s);
						  break;
					  }
				case 'x': {
						  char *x = htoa(va_arg(args, unsigned int));
						  vga_write(x);
						  break;
					  }
			}
		} 
		else {
			vga_putc(*fmt);
		}
		fmt++;
	}
	va_end(args);
}
