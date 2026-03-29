#include <drivers/vga.h>

volatile uint16_t *VGA = (volatile uint16_t *)0xB8000;

uint8_t CURSOR_X = 0;
uint8_t CURSOR_Y = 0;
uint8_t COLOR = 0x0F;

static inline uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(char c, uint8_t color) {
	return (uint16_t)c | (uint16_t)color << 8;
}

static inline int vga_index(uint8_t x, uint8_t y) {
	return y * VGA_WIDTH + x; 
}

void vga_putc(char c) {
	if (CURSOR_Y >= VGA_HEIGHT)
		return;
	if (c == '\n') {
		CURSOR_X = 0;
		CURSOR_Y++;
		return;
	}
	int index = vga_index(CURSOR_X, CURSOR_Y);
	VGA[index] = vga_entry(c, COLOR);
	CURSOR_X++;
	if (CURSOR_X >= VGA_WIDTH) {
		CURSOR_X = 0;
		CURSOR_Y++;
	}
}

void vga_clear() {
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		VGA[i] = vga_entry(' ', COLOR);
	CURSOR_X = 0;
	CURSOR_Y = 0;
}

void vga_write(char *s) {
	while(*s) {
		vga_putc(*s);
		s++;
	}
}

void vga_setcolor(vga_color_t fg, vga_color_t bg) {
	COLOR = vga_entry_color(fg, bg);
}
