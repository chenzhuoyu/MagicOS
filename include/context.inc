;
;  context.inc
;
;   Created on: 2012-11-19
;       Author: desperedo
;
;	thread switcher context
;

K_CONTEXT_BASE		equ 8

; Stack frame

K_GS				equ K_CONTEXT_BASE
K_FS				equ K_GS				+ 4
K_ES				equ K_FS				+ 4
K_DS				equ K_ES				+ 4
K_EDI				equ K_DS				+ 4
K_ESI				equ K_EDI				+ 4
K_EBP				equ K_ESI				+ 4
K_ESP_K				equ K_EBP				+ 4
K_EBX				equ K_ESP_K				+ 4
K_EDX				equ K_EBX				+ 4
K_ECX				equ K_EDX				+ 4
K_EAX				equ K_ECX				+ 4
K_RET_ADDR			equ K_EAX				+ 4
K_EIP				equ K_RET_ADDR			+ 4
K_CS				equ K_EIP				+ 4
K_EFLAGS			equ K_CS				+ 4
K_ESP				equ K_EFLAGS			+ 4
K_SS				equ K_ESP				+ 4

K_CONTEXT_TOP		equ K_SS				+ 4
K_FPU_CONTEXT_TOP	equ K_CONTEXT_TOP		+ 512

K_LDT_SELECTOR		equ K_FPU_CONTEXT_TOP	+ 4
K_LDT				equ	K_LDT_SELECTOR		+ 4

; TSS fields
K_TSS_ESP_0			equ 4