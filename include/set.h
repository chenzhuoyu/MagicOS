/*
 * set.h
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	bit set
 */

#ifndef __SET_H__
#define __SET_H__

#include <global.h>
#include <stdlib.h>

typedef struct _set_t
{
	ulong bits[8];
} set_t;

static inline void set_init(set_t *set)
{
	memset(&set, 0, sizeof(set_t));
}

static inline uchar set_empty(set_t *set)
{
	if (set->bits[0]) return 0;
	if (set->bits[1]) return 0;
	if (set->bits[2]) return 0;
	if (set->bits[3]) return 0;
	if (set->bits[4]) return 0;
	if (set->bits[5]) return 0;
	if (set->bits[6]) return 0;
	if (set->bits[7]) return 0;

	return 1;
}

static inline void set_set(set_t *set, uchar bit)
{
	set->bits[bit >> 5] |= (1 << (bit & 0x1F));
}

static inline void set_clear(set_t *set, uchar bit)
{
	set->bits[bit >> 5] &= ~(1 << (bit & 0x1F));
}

static inline uchar set_test(set_t *set, uchar bit)
{
	return (set->bits[bit >> 5] & (1 << (bit & 0x1F))) != 0;
}

#endif /* __SET_H__ */