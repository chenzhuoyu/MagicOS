;
;  restart.asm
;
;   Created on: 2012-11-18
;       Author: desperedo
;
;	process and thread scheduler starter
;

global sched_start

global restart
global restart_nested

extern k_tss
extern k_current_thread

extern k_int_nested

%define K_INDEX_OF(selector)				((selector) >> 3)
%define K_SELECTOR(index, attr)				((index) << 3 | (attr))

%define K_GDTS_VIDEO						K_SELECTOR(1, K_SA_TI_GDT | K_SA_RPL_3)
%define K_GDTS_FLAT_X						K_SELECTOR(2, K_SA_TI_GDT | K_SA_RPL_0)
%define K_GDTS_FLAT_RW						K_SELECTOR(3, K_SA_TI_GDT | K_SA_RPL_0)
%define K_GDTS_TSS							K_SELECTOR(4, K_SA_TI_GDT | K_SA_RPL_0)
%define K_GDTS_LDT_FIRST					K_SELECTOR(5, K_SA_TI_GDT | K_SA_RPL_0)

%include "context.inc"

; sched_start
sched_start:

; restart
restart:
	mov		esp, dword [k_current_thread]
	lldt	[esp + K_LDT_SELECTOR]
	lea		eax, [esp + K_CONTEXT_TOP]
	mov		dword [k_tss + K_TSS_ESP_0], eax	; Set kernel esp
	add		esp, K_CONTEXT_BASE
	fxrstor	[esp + K_CONTEXT_TOP - K_CONTEXT_BASE]
	fwait
; restart_nested
restart_nested:
	dec		dword [k_int_nested]
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad
	add		esp, 4								; Skip ret_addr field
	iretd