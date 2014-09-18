/*
 * mm.c
 *
 *  Created on: 2012-11-30
 *      Author: desperedo
 *
 *	memory management unit
 */

#include <mm.h>

#include <console.h>

static const ulong * const g_mem_ard_count = (ulong *)0x00090000;
static const ard_item_t * const g_mem_ard_buffer = (ard_item_t *)0x00090004;

ulong k_ram_size = 0;

k_page_item_t *k_pte = (k_page_item_t *)0x00101000;
k_page_item_t *k_pde = (k_page_item_t *)0x00100000;

void init_mm()
{
	for (int i = 0; i < *g_mem_ard_count; i++)
	{
		const ard_item_t *item = &g_mem_ard_buffer[i];

		k_printf("%#.8X:%#.8X  %#.8X:%#.8X  %d\n", item->base_high, item->base_low, item->length_high, item->length_low, item->type);
	}
}