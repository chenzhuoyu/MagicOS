org 0x7C00

; fat_descriptor
fat_descriptor:
	jmp		boot_entry								; JMP instruction,
	nop												; NOP needed

FAT_OEM_NAME				db "MagicBox"			; OEM name
FAT_BYTES_PER_SECTOR		dw 512					; Bytes per sector
FAT_SECTORS_PER_CLUSTER		db 1					; Sectors per cluster
FAT_RESERVED_SECTORS		dw 1					; Reserved sectors
FAT_ENTRIES_COUNT			db 2					; FAT entries count
FAT_ROOT_ENTRIES_COUNT		dw 224					; Root directories entries count
FAT_TOTAL_SECTORS_16		dw 2880					; Total sectors count
FAT_MEDIA_DESC				db 0xF0					; Media descriptor
FAT_FAT_SIZE				dw 9					; FAT size
FAT_SECTORS_PER_TRACK		dw 18					; Sectors per track
FAT_HEADS_COUNT				dw 2					; Magnatic heads count
FAT_HIDDEN_SECTORS			dd 0					; Hidden sectors count
FAT_TOTAL_SECTORS_32		dd 0					; Total sectors count (32bit)

BOOT_DRIVER_NUM				db 0					; Driver number of interrupt 13h
BOOT_RESERVED				db 0					; Reserved
BOOT_SIGNATURE				db 0x29					; Boot signature
BOOT_VOLUME_ID				dd 0					; Volume ID
BOOT_VOLUME_LABEL			db "Magix-v0.1a"		; Volume label
BOOT_FS_TYPE				db "FAT12   "			; Filesystem type

; boot_entry
boot_entry:
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov		ss, ax

	jmp		main

%include "colors.inc"
%include "puts_rm.inc"

; Loader informations

%define STACK_BASE			0x7C00

%define LOADER_BASE			0x9D00
%define LOADER_OFFSET		0x0100

%define SECTOR_COUNT_ROOT	14
%define SECTOR_ROOT_DIR		19
%define SECTOR_FAT_1		1
%define SECTOR_DELTA		17

; Strings

$Point						db '.'
$EndLine					db CR, LF

$LoaderFileName				db "OSLDR      "

$LoadingMsg					db "Loading 'osldr' "
$LoadingMsgLen				equ ($ - $LoadingMsg)

$LoaderNotFound				db CR, LF, CR, LF, "* 'osldr' is missing."
$LoaderNotFoundLen			equ ($ - $LoaderNotFound)

; Variables

$IsOdd						db 0
$RootDir					dw SECTOR_COUNT_ROOT
$SectorNo					dw 0

; main
main:
	mov		sp, STACK_BASE

	; Clear screen
	xor		cx, cx
	mov		ax, 0x0600
	mov		bx, 0x0700
	mov		dx, 0x184F
	int		0x10

	; Print loading message
	print	$LoadingMsg, $LoadingMsgLen, WHITE

	; Reset floopy driver
	xor		ah, ah
	xor		dl, dl
	int		0x13

	; Searching from root
	mov		word [$SectorNo], SECTOR_ROOT_DIR
@do_search:
	cmp		word [$RootDir], 0
	jz		@loader_not_found
	dec		word [$RootDir]

	mov		ax, LOADER_BASE
	mov		es, ax							; Loader base
	mov		bx, LOADER_OFFSET				; Loader offset
	mov		ax, word [$SectorNo]			; Sector number
	mov		cl, 1							; Sectors to read

	call	read_sector

	mov		di, LOADER_OFFSET
	mov		si, $LoaderFileName
	cld
	mov		dx, 0x10
@search_loader:
	or		dx, dx
	jz		@next_sector
	dec		dx
	mov		cx, 11
@compare_str:
	or		cx, cx
	jz		@loader_found
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
	mov		si, $LoaderFileName
	jmp		@search_loader
@next_sector:
	inc		word [$SectorNo]
	jmp		@do_search
@loader_not_found:
	print	$LoaderNotFound, $LoaderNotFoundLen, LIGHT_RED
	jmp		$
@loader_found:
	mov		ax, SECTOR_COUNT_ROOT
	and		di, 0xFFE0

	add		di, 0x1A						; Start sector
	mov		cx, word [es:di]
	push	cx
	add		cx, ax
	add		cx, SECTOR_DELTA

	mov		ax, LOADER_BASE
	mov		es, ax							; Loader base
	mov		bx, LOADER_OFFSET				; Loader offset
	mov		ax, cx							; Sector number
@do_load:
	print	$Point, 1, WHITE

	mov		cl, 1							; Sectors to read
	call	read_sector

	pop		ax
	call	get_fat_entry
	cmp		ax, 0x0FFF
	jz		@load_done
	push	ax

	mov		dx, SECTOR_COUNT_ROOT
	add		ax, dx
	add		ax, SECTOR_DELTA
	add		bx, word [FAT_BYTES_PER_SECTOR]
	jmp		@do_load
@load_done:
	print	$EndLine, 2, WHITE

	jmp		LOADER_BASE:LOADER_OFFSET

; read_sector
; @arg ax		start sector
; @arg cx		count
; @arg es:bx	buffer
read_sector:
	push	ebp
	mov		ebp, esp
	sub		esp, 2							; Reserve spaces for cl

	mov		byte [ebp - 2], cl				; Save sectors count

	push	bx
	mov		bl, byte [FAT_SECTORS_PER_TRACK]
	div		bl
	inc		ah
	mov		cl, ah							; Start sector
	mov		dh, al
	shr		al, 1

	mov		ch, al							; Cluster number
	and		dh, 1							; Magnatic header number
	mov		dl, byte [BOOT_DRIVER_NUM]		; Driver index

	pop		bx
@do_read:
	mov		ah, 2
	mov		al, byte [ebp - 2]				; Sectors to read

	int		0x13
	jc		@do_read

	add		esp, 2							; Restore stack
	pop		ebp
	ret

; get_fat_entry
; @arg ax		sector
; @ret ax		index
get_fat_entry:
	push	es
	push	bx
	push	ax

	mov		ax, LOADER_BASE
	sub		ax, 0x0100						; Reserve 4k spaces for FAT
	mov		es, ax

	pop		ax
	mov		bx, 3
	mul		bx
	mov		bx, 2
	div		bx
	mov		byte [$IsOdd], dl
@even:
	xor		dx, dx							; ax = FAT offset
	mov		bx, word [FAT_BYTES_PER_SECTOR]
	div		bx
	push	dx
	xor		bx, bx
	add		ax, SECTOR_FAT_1				; ax = Sector number of FAT entry

	mov		cl, 2
	call	read_sector

	pop		dx
	add		bx, dx
	mov		ax, word [es:bx]
	cmp		byte [$IsOdd], 0
	jz		@skip
	shr		ax, 4
@skip:
	and		ax, 0x0FFF
@done:
	pop		bx
	pop		es
	ret

	times	510 - ($ - $$) db 0
	dw		0xAA55
