; SPDX-License-Identifier: GPL-2.0
;
; idt.asm - ISR and IRQ stub entry points
;

bits 32

extern isr_handler
extern irq_handler

; Exceptions without a CPU-pushed error code: push dummy 0 first
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
	push dword 0
	push dword %1
	jmp isr_common
%endmacro

; Exceptions where the CPU already pushed an error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
	push dword %1
	jmp isr_common
%endmacro

; Hardware IRQ stubs (%1 = irq index 0-15, %2 = vector 32-47)
%macro IRQ_STUB 2
global irq%1
irq%1:
	push dword 0
	push dword %2
	jmp irq_common
%endmacro

; Exception stubs
ISR_NOERRCODE  0		; #DE divide error
ISR_NOERRCODE  1		; #DB debug
ISR_NOERRCODE  2		; NMI
ISR_NOERRCODE  3		; #BP breakpoint
ISR_NOERRCODE  4		; #OF overflow
ISR_NOERRCODE  5		; #BR bound range
ISR_NOERRCODE  6		; #UD invalid opcode
ISR_NOERRCODE  7		; #NM device not available
ISR_ERRCODE    8		; #DF double fault (error code = 0)
ISR_ERRCODE   10		; #TS invalid TSS
ISR_ERRCODE   11		; #NP segment not present
ISR_ERRCODE   12		; #SS stack-segment fault
ISR_ERRCODE   13		; #GP general protection
ISR_ERRCODE   14		; #PF page fault

; IRQ stubs
IRQ_STUB  0, 32
IRQ_STUB  1, 33
IRQ_STUB  2, 34
IRQ_STUB  3, 35
IRQ_STUB  4, 36
IRQ_STUB  5, 37
IRQ_STUB  6, 38
IRQ_STUB  7, 39
IRQ_STUB  8, 40
IRQ_STUB  9, 41
IRQ_STUB 10, 42
IRQ_STUB 11, 43
IRQ_STUB 12, 44
IRQ_STUB 13, 45
IRQ_STUB 14, 46
IRQ_STUB 15, 47

;
; Common ISR path
; Stack on entry: [int_num] [err_code] [eip] [cs] [eflags]
;
isr_common:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10		; kernel data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	push esp		; isr_frame *
	call isr_handler
	add esp, 4
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8		; discard int_num + err_code
	iret

;
; Common IRQ path
;
irq_common:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	push esp		; isr_frame *
	call irq_handler
	add esp, 4
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret
