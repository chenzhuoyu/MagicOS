/*
 * mm.h
 *
 *  Created on: 2012-11-30
 *      Author: desperedo
 *
 *	memory management unit
 */

#ifndef __MM_H__
#define __MM_H__

#include <global.h>

typedef struct _k_ard_item_t
{
	ulong	base_low;
	ulong	base_high;
	ulong	length_low;
	ulong	length_high;
	ulong	type;
} ard_item_t;

typedef union _k_page_item_t
{
	ulong		item;

	struct
	{
		uchar	attr;
		uchar	addr_3_0_os;
		ushort	addr_19_12;
	};
} k_page_item_t;

extern ulong k_ram_size;

extern k_page_item_t *k_pte;
extern k_page_item_t *k_pde;

void init_mm();

#endif /* __MM_H__ */