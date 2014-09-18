/*
 * stdio.h
 *
 *  Created on: 2012-11-25
 *      Author: desperedo
 */

#ifndef __STDIO_H__
#define __STDIO_H__

#include <global.h>

void setattr(char attr);
void setxy(int x, int y);

int puts(const char *s);
int printf(const char *fmt, ...);

#endif /* __STDIO_H__ */