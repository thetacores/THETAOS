#include <stdint.h>
#include <drivers/vga.h>

void kernel_main(uint32_t multiboot_magic, void *mbi)
{
	(void)multiboot_magic;
	(void)mbi;

	vga_setcolor(VGA_LIGHT_BROWN, VGA_BLUE);
	vga_clear();

	vga_write("KERNEL LOADED!\n");
	

	for (;;)
		__asm__ volatile("cli; hlt");
}
