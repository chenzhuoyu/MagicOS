/*
 * stdarg.h
 *
 *  Created on: 2012-11-10
 *      Author: desperedo
 *
 *	standard argument operations
 */

#ifndef __STDARG_H__
#define __STDARG_H__

#include <global.h>

#define _INTSIZEOF(n)		((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, v)		(ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap, t)		(*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap)			(ap = (va_list)NULL)

typedef char * va_list;

#endif /* __STDARG_H__ */