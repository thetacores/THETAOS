// SPDX-License-Identifier: GPL-2.0
/*
 * kernel.c - THETAOS kernel entry point
 */

#include <stdint.h>
#include <drivers/vga.h>
#include <klib/string.h>
#include <klib/kprintf.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <drivers/keyboard.h>

void kernel_main(uint32_t multiboot_magic, void *mbi)
{
	(void)multiboot_magic;
	(void)mbi;

	gdt_init();
	idt_init();
	keyboard_init();

	vga_setcolor(VGA_LIGHT_BROWN, VGA_BLUE);
	vga_clear();

	kprintf("KERNEL LOADED!\n");
	gdt_debug();

	for (;;) {
		char c = keyboard_getc();
		if (c)
			kprintf("%c", c);
		__asm__ volatile("hlt");
	}
}
