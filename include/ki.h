/*
 * ki.h
 *
 *  Created on: 2012-11-27
 *      Author: desperedo
 *
 *	kernel internal caller stubs
 */

#ifndef __KI_H__
#define __KI_H__

#include <global.h>

void ki_wait(ulong *result);
void ki_exit(ulong *result, ulong exit_code);
void ki_puts(ulong *result, ulong *length, const char *s);
void ki_setattr(ulong *result, ulong attr);
void ki_setxy(ulong *result, ulong position);

#endif /* __KI_H_ */