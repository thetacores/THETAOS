/* SPDX-License-Identifier: GPL-2.0 */
#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt_desc {
	uint16_t limit0;
	uint16_t base0;
	uint8_t  base16;
	uint8_t  access;
	uint8_t  flags_limit;
	uint8_t  base24;
} __attribute__((packed));

struct gdtr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void gdt_init(void);
void gdt_debug(void);

#endif
