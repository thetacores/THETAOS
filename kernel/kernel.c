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
#include <drivers/timer.h>
#include <shell/shell.h>
#include <klib/kmalloc.h>

void kernel_main(uint32_t multiboot_magic, void *mbi)
{
	(void)multiboot_magic;
	(void)mbi;

	kmalloc_init();
	gdt_init();
	idt_init();
	timer_init();
	keyboard_init();

	vga_setcolor(VGA_LIGHT_BROWN, VGA_BLUE);
	vga_clear();

	kprintf("KERNEL MAIN LOADED!\n");

	shell();	
}
