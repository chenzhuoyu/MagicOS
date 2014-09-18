/*
 * stdio.c
 *
 *  Created on: 2012-11-25
 *      Author: desperedo
 *
 *	standard io library
 */

#include <stdio.h>

#include <errors.h>
#include <syscall.h>
#include <vsprintf.h>
#include <stdarg.h>
#include <ki.h>

static char g_print_buffer[1024] = {0};

void setattr(char attr)
{
	ulong result;

	ki_setattr(&result, attr);
}

void setxy(int x, int y)
{
	ulong result;

	ki_setxy(&result, MakeLong(x, y));
}

int puts(const char *s)
{
	ulong result, length;

	ki_puts(&result, &length, s);

	return result == K_ERR_OK ? length : -1;
}

int printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsprintf(g_print_buffer, fmt, args);
	va_end(args);

	return puts(g_print_buffer);
}