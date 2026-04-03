#include <stdint.h>
#include <drivers/vga.h>
#include <klib/string.h>
#include <klib/kprintf.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

void kernel_main(uint32_t multiboot_magic, void *mbi)
{
	(void)multiboot_magic;
	(void)mbi;

	gdt_init();
	//idt_init();

	vga_setcolor(VGA_LIGHT_BROWN, VGA_BLUE);
	vga_clear();

	kprintf("KERNEL LOADED!\n");
	gdt_debug();
	
	for (;;)
		__asm__ volatile("cli; hlt");
}
