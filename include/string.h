/*
 * string.h
 *
 *  Created on: 2012-11-10
 *      Author: desperedo
 */

#ifndef __STRING_H__
#define __STRING_H__

#include <global.h>

int is_digit(char c);

int strlen(const char *s);
char *strcpy(char *dest, const char *src);

int sprintf(char *buf, const char *fmt, ...);

#endif /* __STRING_H__ */