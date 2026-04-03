// SPDX-License-Identifier: GPL-2.0
/*
 * shell.c - THETAOS shell
 */

#include <shell/shell.h>
#include <klib/kprintf.h>
#include <klib/string.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>

void shell_prompt(void)
{
	kprintf("THETAOS> ");
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
			if (c == '\n')	break;
			
			if (c == '\b')
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
		kprintf("\n%s\n", buffer);
	}
}
