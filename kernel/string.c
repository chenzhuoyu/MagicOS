/*
 * string.c
 *
 *  Created on: 2012-11-10
 *      Author: desperedo
 *
 *	string operations
 */

#include <string.h>

#include <stdarg.h>
#include <vsprintf.h>

int is_digit(char c)
{
	return c >= '0' && c <= '9';
}

int strlen(const char *s)
{
	int n = 0;

	while (*s++) n++;

	return n;
}

char *strcpy(char *dest, const char *src)
{
	__asm__ volatile
	(
		".1:"				"\r\n"
		"lodsb"				"\r\n"
		"stosb"				"\r\n"
		"test	%%al, %%al"	"\r\n"
		"loopne	.1"			"\r\n"
		: "=D"(dest)
		: "S"(src)
		:
	);

	return dest;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int n = vsprintf(buf, fmt, args);
	va_end(args);

	return n;
}