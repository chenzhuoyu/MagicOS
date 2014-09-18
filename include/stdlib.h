/*
 * stdlib.h
 *
 *  Created on: 2012-11-7
 *      Author: desperedo
 *
 *	standard C library for Magix
 */

#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <global.h>

int rand();
void srand(uint seed);

void *memset(void *ptr, int value, size_t size);
void *memcpy(void *dest, const void *src, size_t size);
void *memmove(void *dest, const void *src, size_t size);

#endif /* __STDLIB_H__ */