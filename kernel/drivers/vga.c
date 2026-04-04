// SPDX-License-Identifier: GPL-2.0
/*
 * vga.c - VGA text-mode driver
 */

#include <drivers/vga.h>

#define VGA_CTRL 0x3D4                                  
#define VGA_DATA 0x3D5

volatile uint16_t *VGA = (volatile uint16_t *)0xB8000;

static uint8_t cursor_x;
static uint8_t cursor_y;
static uint8_t color = 0x0F;

static inline void outb(uint16_t port, uint8_t val) 
{
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));                                            
}

static inline uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(char c, uint8_t col)
{
	return (uint16_t)c | (uint16_t)col << 8;
}

static inline int vga_index(uint8_t x, uint8_t y)
{
	return y * VGA_WIDTH + x;
}

void vga_hwcursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outb(VGA_CTRL, 0x0F);
	outb(VGA_DATA, (uint8_t)(pos & 0xFF));
	outb(VGA_CTRL, 0x0E);
	outb(VGA_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

static void vga_scroll(void)
{
	int x, y;

	for (y = 1; y < VGA_HEIGHT; y++)
		for (x = 0; x < VGA_WIDTH; x++)
			VGA[vga_index(x, y - 1)] = VGA[vga_index(x, y)];
	for (x = 0; x < VGA_WIDTH; x++)
		VGA[vga_index(x, VGA_HEIGHT - 1)] = vga_entry(' ', color);
	cursor_y = VGA_HEIGHT - 1;
}

void vga_setcolor(vga_color_t fg, vga_color_t bg)
{
	color = vga_entry_color(fg, bg);
}

void vga_setcursor(uint8_t x, uint8_t y)
{
	cursor_x = x;
	cursor_y = y;
	vga_hwcursor(x, y);
}

void vga_clear(void)
{
	int i;

	for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		VGA[i] = vga_entry(' ', color);
	cursor_x = 0;
	cursor_y = 0;
	vga_hwcursor(cursor_x, cursor_y);
}

void vga_putc(char c)
{
	if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
		if (cursor_y >= VGA_HEIGHT)
			vga_scroll();
		vga_hwcursor(cursor_x, cursor_y);
		return;
	}

	VGA[vga_index(cursor_x, cursor_y)] = vga_entry(c, color);
	cursor_x++;
	if (cursor_x >= VGA_WIDTH) {
		cursor_x = 0;
		cursor_y++;
		if (cursor_y >= VGA_HEIGHT)
			vga_scroll();
	}
	vga_hwcursor(cursor_x, cursor_y);
}

void vga_write(char *s)
{
	while (*s) {
		vga_putc(*s);
		s++;
	}
}

void vga_backspace(void)
{
	if (cursor_x == 0)
		return;
	cursor_x--;
	VGA[vga_index(cursor_x, cursor_y)] = vga_entry(' ', color);
	vga_hwcursor(cursor_x, cursor_y);
}
