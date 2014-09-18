/*
 * stdlib.c
 *
 *  Created on: 2012-11-7
 *      Author: desperedo
 *
 *	standard C library for Magix
 */

#include <stdlib.h>

static uint g_seed = 1;

int rand()
{
	 return ((g_seed = g_seed * 214013L + 2531011L) >> 16) & 0x7FFF;
}

void srand(uint seed)
{
	g_seed = seed;
}

void *memset(void *ptr, int value, size_t size)
{
	__asm__ volatile
	(
		"cld"						"\r\n"
		"rep	stosb"				"\r\n"
		:
		: "a"(value), "c"(size), "D"(ptr)
		:
	);

	return ptr;
}

void *memcpy(void *dest, const void *src, size_t size)
{
	__asm__ volatile
	(
		"cld"						"\r\n"
		"mov	%%ecx, %%ebx"		"\r\n"
		"and	$3, %%ecx"			"\r\n"
		"rep	movsb"				"\r\n"
		"mov	%%ebx, %%ecx"		"\r\n"
		"shr	$2, %%ecx"			"\r\n"
		"rep	movsd"				"\r\n"
		:
		: "c"(size), "S"(src), "D"(dest)
		: "%ebx"
	);

	return dest;
}

void *memmove(void *dest, const void *src, size_t size)
{
	if (dest < src) return memcpy(dest, src, size);

	__asm__ volatile
	(
		"std"						"\r\n"
		"mov	%%ecx, %%ebx"		"\r\n"
		"and	$3, %%ecx"			"\r\n"
		"rep	movsb"				"\r\n"
		"mov	%%ebx, %%ecx"		"\r\n"
		"shr	$2, %%ecx"			"\r\n"
		"rep	movsd"				"\r\n"
		:
		: "c"(size), "S"((char *)src + size - 1), "D"((char *)dest + size - 1)
		: "%ebx"
	);

	return dest;
}