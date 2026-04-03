#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* 32-bit protected-mode IDT gate (8 bytes) */
struct idt_desc {
	uint16_t address_low;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  flags;
	uint16_t address_high;
} __attribute__((packed));

struct idtr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

/* Register frame pushed by ISR/IRQ stubs */
struct isr_frame {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax; /* pusha */
	uint32_t int_num, err_code;
	uint32_t eip, cs, eflags; /* pushed by CPU */
} __attribute__((packed));

void idt_init(void);

#endif
