; puts function in real-mode

%define CR					0x0D
%define LF					0x0A

%macro print 3				; print str, len, color
	pusha
	mov		ax, %1
	mov		cx, %2
	mov		bx, %3
	call	puts_rm
	popa
%endmacro

; puts_rm
; @arg ax	Pointer to the string
; @arg cx	Length of the string
; @arg bx	Color
puts_rm:
	push	ax
	push	cx
	mov		ah, 0x03
	int		0x10
	pop		cx
	pop		ax
	push	es
	mov		bp, ax
	mov		ax, ds
	mov		es, ax
	mov		ax, 0x1301
	int		0x10
	pop		es
	ret
