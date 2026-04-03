; SPDX-License-Identifier: GPL-2.0
;
; boot.asm - Multiboot2 header and kernel entry point
;

bits 32

MULTIBOOT2_MAGIC	equ 0xE85250D6
MULTIBOOT_ARCH		equ 0			; i386 protected mode
HEADER_LENGTH		equ (multiboot2_header_end - multiboot2_header_start)
CHECKSUM		equ -(MULTIBOOT2_MAGIC + MULTIBOOT_ARCH + HEADER_LENGTH)

section .multiboot2
align 8
multiboot2_header_start:
	dd MULTIBOOT2_MAGIC
	dd MULTIBOOT_ARCH
	dd HEADER_LENGTH
	dd CHECKSUM
	; end tag
	dw 0			; type
	dw 0			; flags
	dd 8			; size
multiboot2_header_end:

section .bss
align 16
stack_bottom:
	resb 16384		; 16 KiB stack
stack_top:

section .text
global _start
extern kernel_main

_start:
	; Set up the stack
	mov esp, stack_top

	; Push Multiboot2 info for kernel_main(uint32_t magic, void *mbi)
	push ebx		; Multiboot info structure pointer
	push eax		; Multiboot2 magic number

	call kernel_main

	; Halt if kernel_main returns
.hang:
	cli
	hlt
	jmp .hang
