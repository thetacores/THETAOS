// SPDX-License-Identifier: GPL-2.0
/*
 * shell.c - THETAOS shell
 */

#include <shell/shell.h>
#include <klib/kprintf.h>
#include <klib/string.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>

int arg_count(char *buffer) {
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
	kprintf("  shutdown - Power off the system\n");
	kprintf("  reboot   - Restart the system\n");
}

void shell_shutdown(void)
{
	/* QEMU ACPI shutdown */
	__asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0x604));
	/* Fallback: QEMU older port */
	__asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0xB004));
}

void shell_reboot(void)
{
	/* Pulse keyboard controller reset line */
	__asm__ volatile ("outb %0, %1" : : "a"((uint8_t)0xFE), "Nd"((uint16_t)0x64));
	/* Triple fault fallback */
	__asm__ volatile ("int $0x00");
}

void shell_exec(int argc, char *argv[])
{
	if (!strcmp(argv[0], "help")) shell_help();
	else if (!strcmp(argv[0], "shutdown")) shell_shutdown();
	else if (!strcmp(argv[0], "reboot")) shell_reboot();
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
