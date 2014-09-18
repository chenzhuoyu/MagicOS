/*
 * global.h
 *
 *  Created on: 2012-11-8
 *      Author: desperedo
 *
 *	global definations
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define NULL								0
#define INFINITE							0xFFFFFFFF

#define K_GDT_SIZE							128
#define K_IDT_SIZE							256

// Interrupt privilages

#define K_INT_DPL_0							0
#define K_INT_DPL_1							1
#define K_INT_DPL_2							2
#define K_INT_DPL_3							3

// GDT/LDT segment attribute

#define K_DA_SEG_R							0x90		// r--
#define K_DA_SEG_RW							0x92		// rw-
#define K_DA_SEG_RWA						0x93		// rw- , access
#define K_DA_SEG_X							0x98		// --x
#define K_DA_SEG_RX							0x9A		// r-x
#define K_DA_SEG_X_CONFORM					0x9C		// --x , conforming
#define K_DA_SEG_RX_CONFORM					0x9E		// r-x , conforming

// GDT/LDT selector attribute

#define K_SA_RPL_0							0
#define K_SA_RPL_1							1
#define K_SA_RPL_2							2
#define K_SA_RPL_3							3

#define K_SA_TI_GDT							0x00		// Global selector
#define K_SA_TI_LDT							0x04		// Local selector

// GDT/LDT descriptor attribute

#define K_DA_TYPE_LDT						0x82		// LDT
#define K_DA_TYPE_TASK_GATE					0x85		// Task Gate
#define K_DA_TYPE_386_TSS					0x89		// 32-bit TSS
#define K_DA_TYPE_386_CALL_GATE				0x8C		// 32-bit Call Gate
#define K_DA_TYPE_386_INTERRUPT_GATE		0x8E		// 32-bit Interrupt Gate
#define K_DA_TYPE_386_TRAP_GATE				0x8F		// 33-bit Trap Gate

// Global macros

#define K_INDEX_OF(selector)				((selector) >> 3)
#define K_SELECTOR(index, attr)				((index) << 3 | (attr))

#define K_GDTS_VIDEO						K_SELECTOR(1, K_SA_TI_GDT | K_SA_RPL_0)
#define K_GDTS_FLAT_X						K_SELECTOR(2, K_SA_TI_GDT | K_SA_RPL_0)
#define K_GDTS_FLAT_RW						K_SELECTOR(3, K_SA_TI_GDT | K_SA_RPL_0)
#define K_GDTS_TSS							K_SELECTOR(4, K_SA_TI_GDT | K_SA_RPL_0)
#define K_GDTS_LDT_FIRST					K_SELECTOR(5, K_SA_TI_GDT | K_SA_RPL_0)

#define nop()								__asm__ volatile ("nop":::)

#define cli()								__asm__ volatile ("cli":::)
#define sti()								__asm__ volatile ("sti":::)

#define push(value)							__asm__ volatile ("push %%eax"::"a"(value):)

#define Max(a, b)							((a) > (b) ? (a) : (b))
#define Min(a, b)							((a) < (b) ? (a) : (b))

#define MakeWord(h, l)						(((h) << 8) | (l))
#define MakeLong(h, l)						(((h) << 16) | (l))

#define HiByte(value)						(((value) & 0xFF00) >> 0x08)
#define LoByte(value)						(((value) & 0x00FF) >> 0x00)

#define HiWord(value)						(((value) & 0xFFFF0000) >> 0x10)
#define LoWord(value)						(((value) & 0xFFFF0000) >> 0x00)

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned short ushort;

typedef unsigned long size_t;

typedef void (* int_handler_t)();
typedef void (* syscall_handler_t)();

// IDT item

typedef struct _k_idt_t
{
	ushort	offset_15_0;
	ushort	selector;
	uchar	param_count;
	uchar	attribute;
	ushort	offset_31_16;
} k_idt_t;

// GDT/LDT item

typedef struct _k_gdt_ldt_t
{
	ushort	limit_15_0;
	ushort	base_15_0;
	uchar	base_23_16;
	uchar	attr_1;
	uchar	attr_2_limit_20_16;
	uchar	base_31_24;
} k_gdt_ldt_t;

extern k_idt_t k_idt[K_IDT_SIZE];
extern k_gdt_ldt_t k_gdt[K_GDT_SIZE];

ulong get_physical_addr(ushort selector, ulong virtual);

void init_idt_item(uchar vector, uchar type, int_handler_t handler, uchar dpl);
void init_gdt_ldt_item(uchar vector, ulong base, ulong limit, ushort attribute);

#endif /* __GLOBAL_H__ */