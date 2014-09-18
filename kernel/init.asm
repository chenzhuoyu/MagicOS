;
;  restart.asm
;
;   Created on: 2012-11-7
;       Author: desperedo
;
;	kernel initializer
;

extern main
extern k_gdt
extern k_idt

global _start
global $K_STACK_TOP

[section .bss]

$K_STACK			resb 2048
$K_STACK_TOP		resb 0

[section .data]

$GDTS_FLAT_X		equ 0x10

$GdtPtr				dw 0
					dd 0

$IdtPtr				dw 0
					dd 0

[section .text]

; Magix kernel is starting from here
_start:
	; Switch stack to kernel
	mov		esp, $K_STACK_TOP

	; Switch GDT to kernel
	xor		ecx, ecx
	sgdt	[$GdtPtr]						; save GDT
	mov		cx, word [$GdtPtr]				; $GdtPtr + 0 --> limit
	mov		esi, dword [$GdtPtr + 2]		; $GdtPtr + 2 --> base
	mov		edi, k_gdt
	rep		movsb							; copy the GDT to kernel
	mov		word [$GdtPtr], 0x03FF			; new GDT limit, K_GDT_SIZE * sizeof(k_gdt_t) - 1
	mov		dword [$GdtPtr + 2], k_gdt		; new GDT base
	lgdt	[$GdtPtr]						; reload GDT

	; Load IDT
	mov		word [$IdtPtr], 0x07FF			; IDT limit, K_IDT_SIZE * sizeof(k_idt_t) - 1
	mov		dword [$IdtPtr + 2], k_idt		; IDT base
	lidt	[$IdtPtr]						; load IDT

	; Initialize PIT
	mov		al, 0x34
	mov		dx, 0x0043
	out		dx, al

	mov		al, 0x9C
	mov		dx, 0x0040
	out		dx, al

	mov		al, 0x2E
	mov		dx, 0x0040
	out		dx, al

	; Force CPU to use the new GDT
	jmp		dword $GDTS_FLAT_X:main