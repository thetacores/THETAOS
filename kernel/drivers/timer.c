/* SPDX-License-Identifier: GPL-2.0 */
/*
 * timer.c - PIT timer driver
 */

#include <drivers/timer.h>
#include <cpu/io.h>
#include <klib/kprintf.h>

#include <stdint.h>

#define PIT_CHANNEL0	0x40
#define PIT_CMD		0x43
#define PIT_BASE_FREQ	1193182
#define TICKS_PER_SECOND 1000

volatile uint32_t ticks = 0;

void timer_init(void)
{
	uint16_t divisor = PIT_BASE_FREQ / TICKS_PER_SECOND;

	outb(PIT_CMD, 0x36);			/* channel 0, lo/hi, rate generator */
	outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
	outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
	kprintf("[PIT] timer initialized at %d Hz\n", TICKS_PER_SECOND);
}

void timer_irq(void)
{
	ticks++;
}

void current_ticks(void)
{
	kprintf("\nTicks = %d", ticks);
}

void timer_wait(uint32_t ms)
{
	uint32_t start_ticks = ticks;

	while ((ticks - start_ticks) < ms)
	{
		__asm__ volatile("hlt");
	}
}
