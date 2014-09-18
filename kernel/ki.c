/*
 * ki.c
 *
 *  Created on: 2012-11-27
 *      Author: desperedo
 *
 *	kernel internal caller stubs
 */

#include <ki.h>

#include <syscall.h>

void ki_wait(ulong *result)														{ syscall(SYSCALL_WAIT);		}
void ki_exit(ulong *result, ulong exit_code)									{ syscall(SYSCALL_EXIT);		}
void ki_puts(ulong *result, ulong *length, const char *s)						{ syscall(SYSCALL_PUTS);		}
void ki_setattr(ulong *result, ulong attr)										{ syscall(SYSCALL_SET_ATTR);	}
void ki_setxy(ulong *result, ulong position)									{ syscall(SYSCALL_SET_XY);		}