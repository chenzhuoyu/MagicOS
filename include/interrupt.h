/*
 * interrupt.h
 *
 *  Created on: 2012-11-9
 *      Author: desperedo
 *
 *	interrupt handlers
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <global.h>

#define INT_EXCEPTION_COUNT					20

// IRQs

#define INT_DIVIDE_EXCEPTION				0x00
#define INT_DEBUG_EXCEPTION					0x01
#define INT_NMI								0x02
#define INT_BREAK_POINT						0x03
#define INT_OVERFLOW_EXCEPTION				0x04
#define INT_BOUNDS_EXCEPTION				0x05
#define INT_UNKNOWN_INSTRUCTION				0x06
#define INT_DEVICE_UNAVAILABLE				0x07
#define INT_DOUBLE_FAULT					0x08

#define INT_INVALID_TSS						0x0A
#define INT_SEGMENT_FAULT					0x0B
#define INT_STACK_FAULT						0x0C
#define INT_GENERIC_PROTECTION				0x0D
#define INT_PAGE_FAULT						0x0E

#define INT_MATH_FAULT						0x10

#define INT_IRQ_BASE						0x20
#define INT_IRQ_SYSCALL						0x80

extern ulong volatile k_sys_tick;
extern ulong volatile k_int_nested;

void init_interrupt();

void generic_interrupt_handler(ulong vector);

#endif /* __INTERRUPT_H__ */