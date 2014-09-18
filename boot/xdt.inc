; GDT/LDT descriptor attributes
;
;	DA_		: Descriptor attribute
;	D		: Data segment
;	C		: Code segment
;	S		: System segment
;	R		: Read-only
;	RW		: Read/write
;	A		: Access

%define DA_32_BIT							0x4000	; 32-bit segment
%define DA_LIMIT_4K							0x8000	; Segment graularity = 4k

; Descriptor privillage level

%define DA_DPL_0							0x00	; DPL = 0
%define DA_DPL_1							0x20	; DPL = 1
%define DA_DPL_2							0x40	; DPL = 2
%define DA_DPL_3							0x60	; DPL = 3

; GDT/LDT code/data segment types

%define DA_SEG_R							0x90	; r--
%define DA_SEG_RW							0x92	; rw-
%define DA_SEG_RWA							0x93	; rw- , access
%define DA_SEG_X							0x98	; --x
%define DA_SEG_RX							0x9A	; r-x
%define DA_SEG_X_CONFORM					0x9C	; --x , conforming
%define DA_SEG_RX_CONFORM					0x9E	; r-x , conforming

; GDT/LDT system-segment and gate-descriptor types

%define DA_TYPE_LDT							0x82	; LDT
%define DA_TYPE_TASK_GATE					0x85	; Task Gate
%define DA_TYPE_386_TSS						0x89	; 32-bit TSS
%define DA_TYPE_386_CALL_GATE				0x8C	; 32-bit Call Gate
%define DA_TYPE_386_INTERRUPT_GATE			0x8E	; 32-bit Interrupt Gate
%define DA_TYPE_386_TRAP_GATE				0x8F	; 33-bit Trap Gate

; Selector attributes

%define SA_RPL_0							0
%define SA_RPL_1							1
%define SA_RPL_2							2
%define SA_RPL_3							3

%define SA_TIG								0
%define SA_TIL								4

; GDT/LDT item generator

%macro GDT_LDT_DECLARE_ITEM 3						; GDT_LDT_DECLARE_ITEM : base, limit, attr
	dw (%2) & 0xFFFF
	dw (%1) & 0xFFFF
	db ((%1) >> 16) & 0xFF
	dw (((%2) >> 8) & 0x0F00) | ((%3) & 0xF0FF)
	db ((%1) >> 24) & 0xFF
%endmacro

%macro gdt_ldt_base 2								; gdt_ldt_base : desc, base
	xor		eax, eax
	mov		eax, cs
	shl		eax, 4
	add		eax, %2
	mov		word [%1 + 2], ax
	shr		eax, 16
	mov		byte [%1 + 4], al
	mov		byte [%1 + 7], ah
%endmacro

%macro gdt_ldt_limit 3								; gdt_ldt_limit : desc, start, end
	xor		eax, eax
	mov		eax, %3
	sub		eax, %2
	dec		eax
	mov		word [%1], ax
	shr		eax, 8
	and		eax, 0x0F00
	and		word [%1 + 5], 0xF0FF
	or		word [%1 + 5], ax
%endmacro
