/* SPDX-License-Identifier: GPL-2.0 */
/*
 * timer.h - PIT timer driver interface
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t ticks;
void timer_init(void);
void current_ticks(void);
void timer_irq(void);
void timer_wait(uint32_t ms);

#endif
