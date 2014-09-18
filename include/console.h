/*
 * console.h
 *
 *  Created on: 2012-11-7
 *      Author: desperedo
 *
 *  text mode video driver for Magix
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <global.h>

#define PORT_VIDEO_CMD		0x03D4
#define PORT_VIDEO_DATA		0x03D5

extern ushort * const k_video_mem;

void k_setattr(uchar attr);
void k_setxy(int x, int y);

void k_cls(uchar attr);

int k_getch();
int k_getche();
int k_gets(char *s);

int k_putchar(int c);
int k_puts(const char *s);

int k_printf(const char *fmt, ...);

#endif /* __CONSOLE_H__ */