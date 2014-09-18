$VideoMem		dw 0

%macro video_init 1
	push	eax
	mov		eax, %1
	mov		dword [$VideoMem], eax
	pop		eax
%endmacro

%macro video_print 3
%endmacro

; video_write
; @arg edi		Pointer to the string
; @arg ecx		Length of the string
; @arg bh		Color
; @arg dl		X
; @arg dh		Y
video_write:
