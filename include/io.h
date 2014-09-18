/*
 * io.h
 *
 *  Created on: 2012-11-7
 *      Author: desperedo
 *
 *	port io functions for Magix
 */

#ifndef __IO_H__
#define __IO_H__

#include <global.h>

#define outp(p, v)				__asm__ volatile ("outb %%al, %%dx":: "a"(v), "d"(p))
#define outpw(p, v)				__asm__ volatile ("outw %%ax, %%dx":: "a"(v), "d"(p))

inline uchar inp(ushort p)		{ uchar _v; __asm__ volatile ("inb %%dx, %%al":"=a"(_v):"d"(p)); return _v; }
inline ushort inpw(ushort p)	{ ushort _v; __asm__ volatile ("inw %%dx, %%ax":"=a"(_v):"d"(p)); return _v; }

#endif /* __IO_H__ */