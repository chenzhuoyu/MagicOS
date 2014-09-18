;
;  irq.asm
;
;   Created on: 2012-11-18
;       Author: desperedo
;
;	interrupt request handler stubs
;

%include "context.inc"

%define INT_DIVIDE_EXCEPTION			0x00
%define INT_DEBUG_EXCEPTION				0x01
%define INT_NMI							0x02
%define INT_BREAK_POINT					0x03
%define INT_OVERFLOW_EXCEPTION			0x04
%define INT_BOUNDS_EXCEPTION			0x05
%define INT_UNKNOWN_INSTRUCTION			0x06
%define INT_DEVICE_UNAVAILABLE			0x07
%define INT_DOUBLE_FAULT				0x08

%define INT_INVALID_TSS					0x0A
%define INT_SEGMENT_FAULT				0x0B
%define INT_STACK_FAULT					0x0C
%define INT_GENERIC_PROTECTION			0x0D
%define INT_PAGE_FAULT					0x0E

%define INT_MATH_FAULT					0x10

%define I8259_EOI						0x20

%define I8259_INT_M_CTL					0x20
%define I8259_INT_M_MASK				0x21
%define I8259_INT_S_CTL					0xA0
%define I8259_INT_S_MASK				0xA1

%define INT_IRQ_BASE					0x20
%define INT_IRQ_SYSCALL					0x80

%macro call_system_interrupt			1
	call	save_context						; Save context of current thread
	sti											; Enable global interrupt
	push	%1
	call	generic_interrupt_handler			; Invoke generic interrupt handler
	add		esp, 4
	cli											; Redisable global interrupt
	ret											; Return to "restart"
%endmacro

%macro call_hardware_interrupt			1
	call	save_context						; Save context of current thread
	in		al, I8259_INT_M_MASK				; Disable this interrupt temporarily
	or		al, (1 << %1)
	out		I8259_INT_M_MASK, al
	mov		al, I8259_EOI						; Send EOI
	out		I8259_INT_M_CTL, al
	sti											; Enable global interrupt
	push	(%1 + INT_IRQ_BASE)
	call	generic_interrupt_handler			; Invoke interrupt handler
	add		esp, 4
	cli											; Redisable global interrupt
	in		al, I8259_INT_M_MASK				; Reenable this interrupt
	and		al, ~(1 << %1)
	out		I8259_INT_M_MASK, al
	ret											; Return to "restart"
%endmacro

%macro call_exception					1
	call_system_interrupt				%1
%endmacro

%macro call_exception_with_error		1
	push	eax									; Save exception code
	mov		eax, dword [esp - 4]
	mov		dword [esp + K_CONTEXT_TOP - K_EIP], eax
	pop		eax
	add		esp, 4

	call_system_interrupt				%1
%endmacro

extern $K_STACK_TOP

extern k_tss
extern k_int_nested
extern k_current_thread

extern restart
extern restart_nested

extern generic_interrupt_handler

; Exception handlers

global divide_exception
global debug_exception
global nmi
global break_point
global overflow_exception
global bounds_exception
global unknown_instruction
global device_unavailable
global double_fault

global invalid_tss
global segment_fault
global stack_fault
global generic_protection
global page_fault

global math_fault

; Hardware interrupt handlers

global irq_sys_tick
global irq_keyboard

; System interrupt handlers

global irq_syscall

; save_context
save_context:
	pushad
	push	ds
	push	es
	push	fs
	push	gs

	; Check if interrupt nested
	mov		ebp, esp					; Start of k_thread_ready->context
	inc		dword [k_int_nested]
	cmp		dword [k_int_nested], 0
	jg		@int_nested

	; Save FPU status
	fxsave	[esp + K_CONTEXT_TOP - K_CONTEXT_BASE]

	; Switch to kernel stack
	mov		esp, $K_STACK_TOP

	push	restart
	jmp		dword [ebp + K_RET_ADDR - K_CONTEXT_BASE]
@int_nested:
	push	restart_nested
	jmp		dword [ebp + K_RET_ADDR - K_CONTEXT_BASE]

; Exception handlers stub

divide_exception:		call_exception					INT_DIVIDE_EXCEPTION
debug_exception:		call_exception					INT_DEBUG_EXCEPTION
nmi:					call_exception					INT_NMI
break_point:			call_exception					INT_BREAK_POINT
overflow_exception:		call_exception					INT_OVERFLOW_EXCEPTION
bounds_exception:		call_exception					INT_BOUNDS_EXCEPTION
unknown_instruction:	call_exception					INT_UNKNOWN_INSTRUCTION
device_unavailable:		call_exception					INT_DEVICE_UNAVAILABLE
double_fault:			call_exception_with_error		INT_DOUBLE_FAULT
invalid_tss:			call_exception_with_error		INT_INVALID_TSS
segment_fault:			call_exception_with_error		INT_SEGMENT_FAULT
stack_fault:			call_exception_with_error		INT_STACK_FAULT
generic_protection:		call_exception_with_error		INT_GENERIC_PROTECTION
page_fault:				call_exception_with_error		INT_PAGE_FAULT
math_fault:				call_exception_with_error		INT_MATH_FAULT

; Hardware interrupt handlers stub
irq_sys_tick: 			call_hardware_interrupt			0
irq_keyboard: 			call_hardware_interrupt			1

; System interrupt handlers stub
irq_syscall:			call_system_interrupt			INT_IRQ_SYSCALL