; SPDX-License-Identifier: GPL-2.0
;
; gdt.asm - GDT flush and segment reload
;

bits 32

global gdt_flush

gdt_flush:
	mov eax, [esp + 4]	; pointer to gdtr
	lgdt [eax]

	mov ax, 0x10		; kernel data selector (GDT entry 2)
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	jmp 0x08:.reload_cs	; far jump to reload CS
.reload_cs:
	ret
