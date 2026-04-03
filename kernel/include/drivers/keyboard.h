/* SPDX-License-Identifier: GPL-2.0 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <input/keycodes.h>
#include <input/input.h>

#define KB_DATA_PORT		0x60
#define KB_STATUS_PORT		0x64

void keyboard_init(void);
void keyboard_irq(void);

/*
 * keyboard_getc - pull events from the input subsystem until a printable
 * key-press is found.  Returns the ASCII character, or 0 if no event
 * is pending.
 */
char keyboard_getc(void);

#endif
