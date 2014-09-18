/*
 * int_vectors.h
 *
 *  Created on: 2012-11-22
 *      Author: desperedo
 *
 *	interrupt vectors
 */

#ifndef __INT_VECTORS_H__
#define __INT_VECTORS_H__

#include <global.h>

typedef struct _int_vector_t
{
	uchar			vector;
	uchar			dpl;
	int_handler_t	handler;
} int_vector_t;

// Assembler stub exports

void divide_exception();
void debug_exception();
void nmi();
void break_point();
void overflow_exception();
void bounds_exception();
void unknown_instruction();
void device_unavailable();
void double_fault();

void invalid_tss();
void segment_fault();
void stack_fault();
void generic_protection();
void page_fault();

void math_fault();

void irq_sys_tick();
void irq_keyboard();
void irq_syscall();

// Interrupt vectors table

static const int_vector_t INTERRUPT_VECTORS[] =
{
	{ INT_DIVIDE_EXCEPTION		, K_INT_DPL_0, divide_exception			},
	{ INT_DEBUG_EXCEPTION		, K_INT_DPL_0, debug_exception			},
	{ INT_NMI					, K_INT_DPL_0, nmi						},
	{ INT_BREAK_POINT			, K_INT_DPL_0, break_point				},
	{ INT_OVERFLOW_EXCEPTION	, K_INT_DPL_0, overflow_exception		},
	{ INT_BOUNDS_EXCEPTION		, K_INT_DPL_0, bounds_exception			},
	{ INT_UNKNOWN_INSTRUCTION	, K_INT_DPL_0, unknown_instruction		},
	{ INT_DEVICE_UNAVAILABLE	, K_INT_DPL_0, device_unavailable		},
	{ INT_DOUBLE_FAULT			, K_INT_DPL_0, double_fault				},

	{ INT_INVALID_TSS			, K_INT_DPL_0, invalid_tss				},
	{ INT_SEGMENT_FAULT			, K_INT_DPL_0, segment_fault			},
	{ INT_STACK_FAULT			, K_INT_DPL_0, stack_fault				},
	{ INT_GENERIC_PROTECTION	, K_INT_DPL_0, generic_protection		},
	{ INT_PAGE_FAULT			, K_INT_DPL_0, page_fault				},

	{ INT_MATH_FAULT			, K_INT_DPL_0, math_fault				},

	{ INT_IRQ_BASE + 0x00		, K_INT_DPL_1, irq_sys_tick				},
	{ INT_IRQ_BASE + 0x01		, K_INT_DPL_1, irq_keyboard				},

	{ INT_IRQ_SYSCALL			, K_INT_DPL_3, irq_syscall				}
};

#endif /* __INT_VECTORS_H__ */