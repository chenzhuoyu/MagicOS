org 0x0100

[bits 16]

; ldr_entry
ldr_entry:
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov		ss, ax

	mov		sp, 0x0100

	jmp		ldr_main

%include "xdt.inc"
%include "colors.inc"
%include "puts_rm.inc"
%include "page_table.inc"

%define KERNEL_BASE				0x1000
%define KERNEL_OFFSET			0x0000			; Kernel is loaded at 1000h:0000h

%define LOADER_PHY_ADDR			0x0009D000
%define KERNEL_PHY_ADDR			0x00010000

%define PDE_BASE				0x00100000
%define PTE_BASE				0x00101000

%define BYTES_PER_SECTOR		512
%define SECTORS_PER_TRACK		18

%define BOOT_DRIVER_NUM			0

%define SECTOR_COUNT_ROOT		14
%define SECTOR_ROOT_DIR			19
%define SECTOR_FAT_1			1
%define SECTOR_DELTA			17

; GDT entries

$GDT				GDT_LDT_DECLARE_ITEM	0				, 0						, 0
$GDT_VIDEO			GDT_LDT_DECLARE_ITEM	0x000B8000		, 0x0000FFFF			, DA_SEG_RW
$GDT_FLAT_X			GDT_LDT_DECLARE_ITEM	0x00000000		, 0x000FFFFF			, DA_SEG_X | DA_32_BIT | DA_LIMIT_4K
$GDT_FLAT_RW		GDT_LDT_DECLARE_ITEM	0x00000000		, 0x000FFFFF			, DA_SEG_RW | DA_32_BIT | DA_LIMIT_4K

; GDT descriptors

$GdtSize			equ ($ - $GDT)
$GdtPtr				dw $GdtSize - 1				; GDT limit
					dd LOADER_PHY_ADDR + $GDT	; GDT base

; GDT selectors

$GDTS_VIDEO			equ ($GDT_VIDEO		- $GDT)
$GDTS_FLAT_X		equ ($GDT_FLAT_X	- $GDT)
$GDTS_FLAT_RW		equ ($GDT_FLAT_RW	- $GDT)

; Stack space

$STACK_SEGMENT		times 1024 db 0
$STACK_SEGMENT_LEN	equ ($ - $STACK_SEGMENT)

; Strings

$Point				db "."
$EndLine			db CR, LF

$KernelFileName		db "OSKERNEL   "

$LoadingMsg			db "Loading 'oskernel' "
$LoadingMsgLen		equ ($ - $LoadingMsg)

$KernelNotFound		db CR, LF, CR, LF, "* 'oskernel' is missing."
$KernelNotFoundLen	equ ($ - $KernelNotFound)

$MemRamSizeRM		db "* RAM size : ", 0
$MemInfoHeaderRM	db "* Memory Map :", CR, LF, CR, LF
					db "    BaseLow     BaseHigh    LengthLow   LengthHigh  Type   ", CR, LF
					db "  ---------------------------------------------------------", CR, LF, 0

$SizeUnitRM			db " Bytes", 0
$MemoryTypesRM		db "OK   ", CR, LF, 0
					db "Resvd", CR, LF, 0
					db "Resvd", CR, LF, 0

; Variables

$IsOdd				db 0
$RootDir			dw SECTOR_COUNT_ROOT
$SectorNo			dw 0

$DisplayPtrRM		dd 0
$DisplayAttrRM		db 0x0F

$RAMSizeRM			dd 0
$ARDItemRM			dd 0, 0, 0, 0, 0

; Hard-coded addresses

$MemSegment			equ 0x9000

$MemARDCount		equ 0x00090000
$MemARDBuffer		equ 0x00090004

; ldr_mem_check
ldr_mem_check:
	push	eax
	push	ebx
	push	ecx
	xor		ebx, ebx
	mov		di, ($MemARDBuffer & 0xFFFF)
@check_loop:
	mov		eax, 0x0000E820					; Service index
	mov		ecx, 20							; Descriptor size
	mov		edx, 0x534D4150					; 'SMAP'
	int		0x15
	jc		@check_fail
	add		di, 20
	inc		dword [$MemARDCount & 0xFFFF]	; ARDS count
	or		ebx, ebx
	jnz		@check_loop
	jmp		@check_done
@check_fail:
	mov		dword [$MemARDCount & 0xFFFF], 0
@check_done:
	pop		ecx
	pop		ebx
	pop		eax
	ret

; ldr_read_sector
; @arg ax		start sector
; @arg cx		count
; @arg es:bx	buffer
ldr_read_sector:
	push	ebp
	mov		ebp, esp
	sub		esp, 2							; Reserve spaces for cl

	mov		byte [ebp - 2], cl				; Save sectors count

	push	bx
	mov		bl, SECTORS_PER_TRACK
	div		bl
	inc		ah
	mov		cl, ah							; Start sector
	mov		dh, al
	shr		al, 1

	mov		ch, al							; Cluster number
	and		dh, 1							; Magnatic header number
	mov		dl, BOOT_DRIVER_NUM				; Driver index

	pop		bx
@do_read:
	mov		ah, 2
	mov		al, byte [ebp - 2]				; Sectors to read

	int		0x13
	jc		@do_read

	add		esp, 2							; Restore stack
	pop		ebp
	ret

; ldr_get_fat_entry
; @arg ax		sector
; @ret ax		index
ldr_get_fat_entry:
	push	es
	push	bx
	push	ax

	mov		ax, KERNEL_BASE
	sub		ax, 0x0100						; Reserve 4k spaces for FAT
	mov		es, ax

	pop		ax
	mov		bx, 3
	mul		bx
	mov		bx, 2
	div		bx
	mov		byte [$IsOdd], dl

	xor		dx, dx							; ax = FAT offset
	mov		bx, BYTES_PER_SECTOR
	div		bx
	push	dx
	xor		bx, bx
	add		ax, SECTOR_FAT_1				; ax = Sector number of FAT entry

	mov		cl, 2
	call	ldr_read_sector

	pop		dx
	add		bx, dx
	mov		ax, word [es:bx]
	cmp		byte [$IsOdd], 0
	jz		@skip
	shr		ax, 4
@skip:
	and		ax, 0x0FFF
	pop		bx
	pop		es
	ret

; ldr_kill_motor
ldr_kill_motor:
	push	dx
	mov		dx, 0x03F2
	mov		al, 0
	out		dx, al
	pop		dx
	ret

; ldr_main
ldr_main:
	print	$LoadingMsg, $LoadingMsgLen, WHITE

	; Memory check at startup
	push	ds
	push	es
	mov		ax, $MemSegment
	mov		ds, ax
	mov		es, ax
	call	ldr_mem_check
	pop		es
	pop		ds

	; Reset floopy driver
	xor		ah, ah
	xor		dl, dl
	int		0x13

	; Searching from root
	mov		word [$SectorNo], SECTOR_ROOT_DIR
@do_search:
	cmp		word [$RootDir], 0
	jz		@kernel_not_found
	dec		word [$RootDir]

	mov		ax, KERNEL_BASE
	mov		es, ax							; Kernel base
	mov		bx, KERNEL_OFFSET				; Kernel offset
	mov		ax, word [$SectorNo]			; Sector number
	mov		cl, 1							; Sectors to read

	call	ldr_read_sector

	mov		di, KERNEL_OFFSET
	mov		si, $KernelFileName
	cld
	mov		dx, 0x10
@search_kernel:
	or		dx, dx
	jz		@next_sector
	dec		dx
	mov		cx, 11
@compare_str:
	or		cx, cx
	jz		@kernel_found
	dec		cx
	lodsb
	cmp		al, byte [es:di]
	jz		@continue
	jmp		@different
@continue:
	inc		di
	jmp		@compare_str
@different:
	and		di, 0xFFE0						; Offset of file name, aligned to 0x20
	add		di, 0x20
	mov		si, $KernelFileName
	jmp		@search_kernel
@next_sector:
	inc		word [$SectorNo]
	jmp		@do_search
@kernel_not_found:
	print	$KernelNotFound, $KernelNotFoundLen, LIGHT_RED
	jmp		$
@kernel_found:
	mov		ax, SECTOR_COUNT_ROOT
	and		di, 0xFFE0

	add		di, 0x1A						; Start sector
	mov		cx, word [es:di]
	push	cx
	add		cx, ax
	add		cx, SECTOR_DELTA

	mov		ax, KERNEL_BASE
	mov		es, ax							; Kernel base
	mov		bx, KERNEL_OFFSET				; Kernel offset
	mov		ax, cx							; Sector number
@do_load:
	print	$Point, 1, WHITE

	mov		cl, 1							; Sectors to read
	call	ldr_read_sector

	pop		ax
	call	ldr_get_fat_entry
	cmp		ax, 0x0FFF
	jz		@load_done
	push	ax

	mov		dx, SECTOR_COUNT_ROOT
	add		ax, dx
	add		ax, SECTOR_DELTA
	add		bx, BYTES_PER_SECTOR
	jmp		@do_load
@load_done:
	call	ldr_kill_motor
	print	$EndLine, 2, WHITE				; The kernel has read into the memory, now place it at the right place

	; Load GDT
	lgdt	[$GdtPtr]

	; Clear interrupts
	cli

	; Open A20 address line
	in		al, 0x92
	or		al, 0x02
	out		0x92, al

	; Enable protect mode, set PE bit of CR0
	mov		eax, cr0
	or		eax, 1
	mov		cr0, eax

	; Jump to 32-bit code
	jmp		dword $GDTS_FLAT_X:(LOADER_PHY_ADDR + ldr_main32)

; 32-bit Code Segment
[bits 32]

; Stack

$STACK				times 1024 db 0			; 1k space for stack
$STACK_TOP			equ (LOADER_PHY_ADDR + $)

; Strings under Protect-Mode

$MemRamSize			equ (LOADER_PHY_ADDR + $MemRamSizeRM)
$MemInfoHeader		equ (LOADER_PHY_ADDR + $MemInfoHeaderRM)

$SizeUnit			equ (LOADER_PHY_ADDR + $SizeUnitRM)
$MemoryTypes		equ (LOADER_PHY_ADDR + $MemoryTypesRM)

; Variables under Protect-Mode

$DisplayPtr			equ (LOADER_PHY_ADDR + $DisplayPtrRM)
$DisplayAttr		equ (LOADER_PHY_ADDR + $DisplayAttrRM)

$RAMSize			equ (LOADER_PHY_ADDR + $RAMSizeRM)
$ARDItem			equ (LOADER_PHY_ADDR + $ARDItemRM)

; setattr
; @arg al	attribute
setattr:
	mov		byte [$DisplayAttr], al
	ret

; setcursor
; @arg bx	address
setcursor:
	push	eax
	push	edx
	mov		al, 14
	mov		dx, 0x03D4
	out		dx, al
	mov		al, bh
	mov		dx, 0x03D5
	out		dx, al
	mov		al, 15
	mov		dx, 0x03D4
	out		dx, al
	mov		al, bl
	mov		dx, 0x03D5
	out		dx, al
	pop		edx
	pop		eax
	ret

; setxy
; @arg eax	y
; @arg ebx	x
setxy:
	pushad
	mov		ecx, 80
	mul		ecx
	xor		edx, edx
	add		eax, ebx
	mov		ebx, eax
	call	setcursor
	shl		eax, 1
	mov		dword [$DisplayPtr], eax
	popad
	ret

; scrollup
scrollup:
	pushad
	push	ds
	push	es
	xor		al, al
	mov		ah, byte [$DisplayAttr]
	push	eax
	mov		eax, gs
	mov		ds, eax
	mov		es, eax
	xor		edi, edi
	mov		esi, 160
	mov		ecx, 80 * 24
	rep		movsw
	pop		eax
	mov		ecx, 80
	rep		stosw
	pop		es
	pop		ds
	sub		dword [$DisplayPtr], 160
	popad
	ret

; putchar
; @arg al	char
putchar:
	pushad
	cmp		al, LF							; "\n"
	jz		@line_feed
	cmp		al, CR							; "\r"
	jz		@carriage_return
	mov		ah, byte [$DisplayAttr]
	mov		edi, dword [$DisplayPtr]
	mov		word [gs:edi], ax
	add		dword [$DisplayPtr], 2
	jmp		@check_scroll
@line_feed:
	add		dword [$DisplayPtr], 160
	jmp		@check_scroll
@carriage_return:
	xor		edx, edx
	mov		edi, 160
	mov		eax, dword [$DisplayPtr]
	div		edi
	sub		dword [$DisplayPtr], edx
@check_scroll:
	cmp		dword [$DisplayPtr], 80 * 25 * 2
	jb		@no_scroll
	call	scrollup
@no_scroll:
	mov		ebx, dword [$DisplayPtr]
	shr		ebx, 1
	call	setcursor
	popad
	ret

; puts
; @arg esi	string
puts:
	push	eax
	push	esi
@next_char:
	lodsb
	or		al, al
	jz		@string_end
	call	putchar
	jmp		@next_char
@string_end:
	pop		esi
	pop		eax
	ret

; putu
; @arg eax	value
putu:
	pushad
	mov		ebx, 10
	xor		ecx, ecx
@save_digit_u:
	inc		ecx
	xor		edx, edx
	div		ebx
	push	edx								; Push the remainder into stack
	or		eax, eax
	jnz		@save_digit_u
@build_str_u:
	pop		eax
	and		eax, 0x0F
	add		eax, "0"						; Convert into ascii
	call	putchar
	loop	@build_str_u
	popad
	ret

; putx
; @arg eax	value
putx:
	push	eax
	push	ecx
	push	eax
	mov		al, '0'
	call	putchar
	mov		al, 'x'
	call	putchar							; Display "0x" prefix
	pop		eax
	mov		ecx, 8
@save_digit_x:
	push	eax								; Push the remainder into stack
	shr		eax, 4
	loop	@save_digit_x
	mov		ecx, 8
@build_str_x:
	pop		eax								; Get saved digit
	and		eax, 0x0F
	cmp		eax, 10
	jnb		@build_hex_x
	add		eax, "0"						; Convert to digit
	jmp		@display_digit_x
@build_hex_x:
	sub		eax, 10
	add		eax, "A"						; Convert to hex
@display_digit_x:
	call	putchar							; Display it
	loop	@build_str_x
	pop		ecx
	pop		eax
	ret

; ldr_get_mem_info
ldr_get_mem_info:
	push	esi
	push	edi
	push	ecx

	mov		al, LIGHT_CYAN
	call	setattr
	mov		esi, $MemInfoHeader				; Information header
	call	puts

	mov		esi, $MemARDBuffer
	mov		ecx, dword [$MemARDCount]
@ard_loop:
	mov		edx, 4
	mov		edi, $ARDItem
	mov		al, ' '
	call	putchar
	call	putchar
	call	putchar
	call	putchar
@ard_item_loop:
	mov		eax, dword [esi]
	stosd									; Store at $ARDItem
	call	putx							; Display as hex
	mov		al, ' '
	call	putchar
	call	putchar
	add		esi, 4
	dec		edx
	or		edx, edx
	jnz		@ard_item_loop

	mov		eax, dword [esi]				; Type field is treated seperately
	stosd
	add		esi, 4

	push	esi								; Display type field
	lea		esi, [$MemoryTypes + eax * 8 - 8]
	call	puts
	pop		esi

	cmp		dword [$ARDItem + 16], 1		; $ARDItem + 16 --> dwType, 1 is RAM

	jne		@next_ard_item
	mov		eax, dword [$ARDItem]			; $ARDItem + 0 --> dwBaseAddrLow
	add		eax, dword [$ARDItem + 8]		; $ARDItem + 8 --> dwLengthLow
	cmp		eax, dword [$RAMSize]			; dwBaseAddrLow + dwLengthLow > $RAMSize ?
	jb		@next_ard_item
	mov		dword [$RAMSize], eax			; Yes, update $RAMSize
@next_ard_item:
	loop	@ard_loop

	mov		al, CR
	call	putchar
	mov		al, LF
	call	putchar							; Return

	mov		esi, $MemRamSize
	call	puts

	mov		eax, dword [$RAMSize]
	call	putu							; Display RAM size
	mov		esi, $SizeUnit
	call	puts

	pop		ecx
	pop		edi
	pop		esi
	ret

; ldr_setup_paging
ldr_setup_paging:
	xor		edx, edx
	mov		eax, dword [$RAMSize]
	mov		ebx, 0x00400000					; 4MB
	div		ebx
	mov		ecx, eax
	or		edx, edx
	jz		@no_remainder
	inc		ecx
@no_remainder:
	push	ecx								; Save the number of PTE
	mov		edi, PDE_BASE
	mov		eax, PTE_BASE | PG_P | PG_RW | PG_USER
@init_pde_item:
	stosd									; Initialize PDE item
	add		eax, 4096
	loop	@init_pde_item

	pop		eax								; PDE count
	mov		ebx, 1024						; 1024 PTE per PDE
	mul		ebx
	mov		ecx, eax
	mov		edi, PTE_BASE
	xor		eax, eax
	mov		eax, PG_P | PG_RW | PG_USER
@init_pte_item:
	stosd									; Initialize PTR item
	add		eax, 4096						; 4k per page
	loop	@init_pte_item

	mov		eax, PDE_BASE
	mov		cr3, eax						; Setting up PDE Base

	mov		eax, cr0
	or		eax, 0x80000000
	mov		cr0, eax						; Enable paging

	jmp		@ret_after_paging				; This JMP is needed
@ret_after_paging:
	ret

; ldr_main32
ldr_main32:
	; Setting up segment registers
	mov		eax, $GDTS_VIDEO
	mov		gs, eax
	mov		eax, $GDTS_FLAT_RW
	mov		ds, eax
	mov		es, eax
	mov		fs, eax
	mov		ss, eax

	; Setting up ESP
	mov		esp, $STACK_TOP

	; Initialize cursor
	mov		eax, 3
	xor		ebx, ebx
	call	setxy

	; Enable fxsave/fxrstor support
	mov		eax, cr4
	or		eax, 0x0204
	mov		cr4, eax

	; Get memory information
	call	ldr_get_mem_info

	; Setting up paging
	call	ldr_setup_paging

	; Place the kernel at a right place
	xor		ecx, ecx
	mov		esi, KERNEL_PHY_ADDR			; ELF header
	mov		eax, dword [esi + 24]			; ELF + 24 --> e_entry
	mov		ebx, @switch_control_over
	add		ebx, LOADER_PHY_ADDR			; Get the real address of the long jump
	mov		dword [ebx + 1], eax			; Fill the entry address
	mov		eax, dword [esi + 28]			; ELF + 28 --> e_phoff
	mov		cx, word [esi + 44]				; ELF + 44 --> e_phnum
	add		esi, eax						; ELF + e_phoff --> PH
@load_section:
	push	ecx
	cmp		dword [esi], 0					; [PH + 0] --> p_type
	jz		@next_section
	push	esi								; Save PH
	mov		edi, dword [esi + 8]			; [PH + 8] --> p_vaddr
	mov		ecx, dword [esi + 16]			; [PH + 16] --> p_filesize
	mov		esi, dword [esi + 4]			; [PH + 4] --> p_offset
	add		esi, KERNEL_PHY_ADDR			; Where the section located
	rep		movsb							; Copy the section to it's right place
	pop		esi
@next_section:
	add		esi, 32							; sizeof(elf32_program) == 32
	pop		ecx
	loop	@load_section

	; Now, the kernel is placed at the right place, just move the control over to our kernel.
@switch_control_over:
	jmp		dword $GDTS_FLAT_X:0x00000000	; The offset address had filled before