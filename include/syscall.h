/*
 * syscall.h
 *
 *  Created on: 2012-11-20
 *      Author: desperedo
 *
 *	syscall handlers
 */

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <global.h>

#define SYSCALL_WAIT					0
#define SYSCALL_EXIT					1
#define SYSCALL_PUTS					2
#define SYSCALL_SET_ATTR				3
#define SYSCALL_SET_XY					4

#define syscall(index)					__asm__ volatile ("int $0x80"::"la"(index):)

void interrupt_syscall();

#endif /* __SYSCALL_H__ */