// SPDX-License-Identifier: GPL-2.0
/*
 * shell.c - THETAOS shell
 */

#include <shell/shell.h>
#include <cpu/io.h>
#include <klib/kprintf.h>
#include <klib/string.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <drivers/ata.h>

int arg_count(char *buffer)
{
	int count = 0;
	int in_word = 0;
	while (*buffer) {
		if (*buffer != ' ') {
			if (!in_word) {
				count++;
				in_word = 1;
			}
		} else {
			in_word = 0;
		}
		buffer++;
	}
	return count;
}

void shell_prompt(void)
{
	kprintf("THETAOS> ");
}

void shell_help(void)
{
	kprintf("THETAOS Shell Commands:\n");
	kprintf("  help     - Show this help message\n");
	kprintf("  clear    - Clear the screen\n");
	kprintf("  credit   - Show the credit\n");
	kprintf("  echo     - Print arguments to the screen\n");
	kprintf("  poweroff - Power off the system\n");
	kprintf("  reboot   - Restart the system\n");
}

void shell_clear(void)
{
	vga_clear();
}

void shell_credit(void)
{
	kprintf("THETAOS - A 32-bit x86 Operating System\n");
	kprintf("Built by thetacores\n");
	kprintf("Toolchain: nasm, i686-elf-gcc, GRUB/Multiboot2\n");
}

void shell_echo(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) 
		kprintf("%s ", argv[i]);
	kprintf("\n");
}


void shell_poweroff(void)
{
	/* QEMU ACPI shutdown */
	outw(0x604, 0x2000);
	/* Fallback: QEMU older port */
	outw(0xB004, 0x2000);
}

void shell_reboot(void)
{
	/* Pulse keyboard controller reset line */
	outb(0x64, 0xFE);
	/* Triple fault fallback */
	__asm__ volatile ("int $0x00");
}

void shell_exec(int argc, char *argv[])
{
	if (!strcmp(argv[0], "help")) shell_help();
	else if (!strcmp(argv[0], "clear")) shell_clear();
	else if (!strcmp(argv[0], "credit")) shell_credit();
	else if (!strcmp(argv[0], "echo")) shell_echo(argc, argv);
	else if (!strcmp(argv[0], "poweroff")) shell_poweroff();
	else if (!strcmp(argv[0], "reboot")) shell_reboot();
	
	/*ATA DEBUG*/
	/*
	else if (!strcmp(argv[0], "write")) shell_ata_write(argc, argv);
	else if (!strcmp(argv[0], "read")) shell_ata_read(argc, argv);
	*/

	else kprintf("%s: Command not found.\n", argv[0]);
}

void shell(void)
{
	for(;;) 
	{	
		char buffer[128];
		int count = 0;

		memset(buffer, 0, sizeof(buffer));	

		shell_prompt();
		for(;;) 
		{
			char c = keyboard_getc();
			if (c == '\n')
			{
				kprintf("\n");
				int argc = arg_count(buffer);
				char *argv[argc];
				int idx = 0;
				char *p = buffer;
				while (*p) 
				{
					if (*p != ' ') 
					{
						argv[idx++] = p;
						while (*p && *p != ' ') p++;
						if (*p) *p++ = '\0';
					} 
					else p++;
				}
				if (argc > 0) 
				{
					shell_exec(argc, argv);
				}
				break;
			}
			else if (c == '\b')
			{
				if (count > 0) 
				{
					buffer[count] = ' ';
					count--;
					vga_backspace();
				}
			}

			else if (c != 0) 
			{
				buffer[count] = c;
				count++;
				kprintf("%c", c);
			}
		}
	}
}
