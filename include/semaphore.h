/*
 * semaphore.h
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	semaphore for thread synchronization
 */

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <global.h>
#include <linked_list.h>

typedef struct _k_semaphore_t
{
	list_entry_t	list;
	ulong			wait;
	ulong			count;
	ulong			max_count;
} k_semaphore_t;

k_semaphore_t *k_sem_new(ulong count);
void k_sem_free(k_semaphore_t *semaphore);

void k_sem_none(k_semaphore_t *semaphore);
void k_sem_signal(k_semaphore_t *semaphore);
uchar k_sem_wait(k_semaphore_t *semaphore, ulong timeout);

#endif /* __SEMAPHORE_H__ */