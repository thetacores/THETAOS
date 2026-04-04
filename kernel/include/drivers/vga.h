/* SPDX-License-Identifier: GPL-2.0 */
#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH		80
#define VGA_HEIGHT		25

typedef enum {
	VGA_BLACK		= 0,
	VGA_BLUE		= 1,
	VGA_GREEN		= 2,
	VGA_CYAN		= 3,
	VGA_RED			= 4,
	VGA_MAGENTA		= 5,
	VGA_BROWN		= 6,
	VGA_LIGHT_GREY		= 7,
	VGA_DARK_GREY		= 8,
	VGA_LIGHT_BLUE		= 9,
	VGA_LIGHT_GREEN		= 10,
	VGA_LIGHT_CYAN		= 11,
	VGA_LIGHT_RED		= 12,
	VGA_LIGHT_MAGENTA	= 13,
	VGA_LIGHT_BROWN		= 14,
	VGA_WHITE		= 15,
} vga_color_t;

void vga_setcolor(vga_color_t fg, vga_color_t bg);
void vga_setcursor(uint8_t x, uint8_t y);
void vga_clear(void);
void vga_putc(char c);
void vga_write(char *s);
void vga_backspace(void);
void vga_disable_cursor(void);

#endif
