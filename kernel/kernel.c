#include <stdint.h>
#include <drivers/vga.h>
#include <klib/string.h>
#include <klib/kprintf.h>

void kernel_main(uint32_t multiboot_magic, void *mbi)
{
	(void)multiboot_magic;
	(void)mbi;

	vga_setcolor(VGA_LIGHT_BROWN, VGA_BLUE);
	vga_clear();

	vga_setcursor(0, 5);
	char welcome[] = "Welcome to THETA-OS!";
	char c = 'X';
	int d = -1087;
	unsigned int x = 0xFFFFFBC1;
	kprintf("%s\n", welcome);
	kprintf("%c\n", c);
	kprintf("%d\n", d);
	kprintf("%x\n", x);

	for (;;)
		__asm__ volatile("cli; hlt");
}
