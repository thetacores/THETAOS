// SPDX-License-Identifier: GPL-2.0
/*
 * idt.c - Interrupt Descriptor Table and 8259 PIC setup
 */

#include <cpu/idt.h>
#include <klib/kprintf.h>
#include <drivers/keyboard.h>

static struct idt_desc idt[256];
static struct idtr     idtr;

/* I/O helpers */
static inline void outb(uint16_t port, uint8_t val)
{
	__asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void io_wait(void)
{
	outb(0x80, 0);
}

/* 8259 PIC */
#define PIC1_CMD		0x20
#define PIC1_DATA		0x21
#define PIC2_CMD		0xA0
#define PIC2_DATA		0xA1
#define PIC_EOI			0x20

static void pic_remap(uint8_t off1, uint8_t off2)
{
	uint8_t m1 = inb(PIC1_DATA);
	uint8_t m2 = inb(PIC2_DATA);

	outb(PIC1_CMD,  0x11); io_wait();	/* ICW1: init + ICW4 */
	outb(PIC2_CMD,  0x11); io_wait();
	outb(PIC1_DATA, off1); io_wait();	/* ICW2: vector offsets */
	outb(PIC2_DATA, off2); io_wait();
	outb(PIC1_DATA, 0x04); io_wait();	/* ICW3: slave on IRQ2 */
	outb(PIC2_DATA, 0x02); io_wait();	/* ICW3: cascade id 2 */
	outb(PIC1_DATA, 0x01); io_wait();	/* ICW4: 8086 mode */
	outb(PIC2_DATA, 0x01); io_wait();

	outb(PIC1_DATA, m1);			/* restore masks */
	outb(PIC2_DATA, m2);
}

static void idt_set_gate(uint8_t vec, uint32_t handler, uint16_t sel,
			 uint8_t flags)
{
	idt[vec].address_low  = (uint16_t)(handler & 0xFFFF);
	idt[vec].address_high = (uint16_t)((handler >> 16) & 0xFFFF);
	idt[vec].selector     = sel;
	idt[vec].zero         = 0;
	idt[vec].flags        = flags;
}

/* C-level handlers (called from idt.asm stubs) */
void isr_handler(struct isr_frame *f)
{
	kprintf("[ISR] exception %d err=0x%x eip=0x%x\n",
		f->int_num, f->err_code, f->eip);
	for (;;)
		__asm__ volatile("cli; hlt");
}

void irq_handler(struct isr_frame *f)
{
	switch (f->int_num) {
	case 33:					/* IRQ1: keyboard */
		keyboard_irq();
		break;
	}

	if (f->int_num >= 40)				/* ACK slave PIC */
		outb(PIC2_CMD, PIC_EOI);
	outb(PIC1_CMD, PIC_EOI);			/* ACK master PIC */
}

void idt_init(void)
{
	extern void isr0(void);  extern void isr1(void);  extern void isr2(void);
	extern void isr3(void);  extern void isr4(void);  extern void isr5(void);
	extern void isr6(void);  extern void isr7(void);  extern void isr8(void);
	extern void isr10(void); extern void isr11(void); extern void isr12(void);
	extern void isr13(void); extern void isr14(void);

	extern void irq0(void);  extern void irq1(void);  extern void irq2(void);
	extern void irq3(void);  extern void irq4(void);  extern void irq5(void);
	extern void irq6(void);  extern void irq7(void);  extern void irq8(void);
	extern void irq9(void);  extern void irq10(void); extern void irq11(void);
	extern void irq12(void); extern void irq13(void); extern void irq14(void);
	extern void irq15(void);

	/* IRQ0-7 -> vectors 32-39, IRQ8-15 -> 40-47 */
	pic_remap(0x20, 0x28);

#define KCS 0x08 /* kernel code selector */
	idt_set_gate( 0, (uint32_t)isr0,  KCS, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1,  KCS, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2,  KCS, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3,  KCS, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4,  KCS, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5,  KCS, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6,  KCS, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7,  KCS, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8,  KCS, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, KCS, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, KCS, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, KCS, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, KCS, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, KCS, 0x8E);

	idt_set_gate(32, (uint32_t)irq0,  KCS, 0x8E);
	idt_set_gate(33, (uint32_t)irq1,  KCS, 0x8E);
	idt_set_gate(34, (uint32_t)irq2,  KCS, 0x8E);
	idt_set_gate(35, (uint32_t)irq3,  KCS, 0x8E);
	idt_set_gate(36, (uint32_t)irq4,  KCS, 0x8E);
	idt_set_gate(37, (uint32_t)irq5,  KCS, 0x8E);
	idt_set_gate(38, (uint32_t)irq6,  KCS, 0x8E);
	idt_set_gate(39, (uint32_t)irq7,  KCS, 0x8E);
	idt_set_gate(40, (uint32_t)irq8,  KCS, 0x8E);
	idt_set_gate(41, (uint32_t)irq9,  KCS, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, KCS, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, KCS, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, KCS, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, KCS, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, KCS, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, KCS, 0x8E);
#undef KCS

	idtr.limit = sizeof(idt) - 1;
	idtr.base  = (uint32_t)idt;
	__asm__ volatile("lidt %0" : : "m"(idtr));
	__asm__ volatile("sti");
}
