/*
 * vsprintf.h
 *
 *  Created on: 2012-11-10
 *      Author: desperedo
 */

#ifndef __VSPRINTF_H__
#define __VSPRINTF_H__

#include <global.h>
#include <stdarg.h>

int vsprintf(char *buf, const char *fmt, va_list args);

#endif /* __VSPRINTF_H__ */
