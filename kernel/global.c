/*
 * global.c
 *
 *  Created on: 2012-11-16
 *      Author: desperedo
 *
 *	global variables
 */

#include <global.h>

k_idt_t		k_idt[K_IDT_SIZE];
k_gdt_ldt_t	k_gdt[K_GDT_SIZE];

ulong get_physical_addr(ushort selector, ulong virtual)
{
	k_gdt_ldt_t *item = &k_gdt[K_INDEX_OF(selector)];

	return ((item->base_31_24 << 24) | (item->base_23_16 << 16) | item->base_15_0) + virtual;
}

void init_idt_item(uchar vector, uchar type, int_handler_t handler, uchar dpl)
{
	k_idt[vector].param_count = 0;

	k_idt[vector].selector = K_GDTS_FLAT_X;
	k_idt[vector].attribute = (dpl << 5) | type;

	k_idt[vector].offset_15_0 = ((ulong)handler) & 0x0000FFFF;
	k_idt[vector].offset_31_16 = (((ulong)handler) & 0xFFFF0000) >> 16;
}

void init_gdt_ldt_item(uchar vector, ulong base, ulong limit, ushort attribute)
{
	k_gdt[vector].attr_1 = attribute & 0x00FF;

	k_gdt[vector].base_15_0 = base & 0x0000FFFF;
	k_gdt[vector].base_23_16 = (base & 0x00FF0000) >> 16;
	k_gdt[vector].base_31_24 = (base & 0xFF000000) >> 24;

	k_gdt[vector].limit_15_0 = limit & 0x0000FFFF;
	k_gdt[vector].attr_2_limit_20_16 = ((limit & 0x000F0000) >> 16) | ((attribute & 0xF000) >> 8);
}