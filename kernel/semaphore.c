/*
 * semaphore.c
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	semaphore for thread synchronization
 */

#include <semaphore.h>

#include <slab.h>
#include <thread.h>
#include <errors.h>
#include <stdlib.h>
#include <ki.h>

k_semaphore_t *k_sem_new(ulong count)
{
	k_semaphore_t *semaphore = slab_allocate(SLAB_SEMAPHORE);

	semaphore->wait = 0;
	semaphore->count = count;
	semaphore->max_count = count;

	return semaphore;
}

void k_sem_free(k_semaphore_t *semaphore)
{
	if (semaphore->wait)
	{
		ulong result;

		k_sem_signal(semaphore);
		ki_wait(&result);
	}

	slab_free(SLAB_EVENT, semaphore);
}

void k_sem_none(k_semaphore_t *semaphore)
{
	cli();
	semaphore->count = 0;
	sti();
}

void k_sem_signal(k_semaphore_t *semaphore)
{
	cli();
	semaphore->count = Min(semaphore->max_count, semaphore->count + 1);
	sti();
}

uchar k_sem_wait(k_semaphore_t *semaphore, ulong timeout)
{
	if (semaphore->count == 0)
	{
		ulong result;
		k_wait_t *wait = slab_allocate(SLAB_WAIT);

		wait->object = semaphore;
		wait->timeout = timeout;

		cli();

		semaphore->wait++;

		set_set(&k_current_thread->wait, WAIT_SEMAPHORES);
		list_append(&k_current_thread->semaphores, (list_entry_t *)wait);

		sti();

		ki_wait(&result);

		return semaphore->count ? (semaphore->count--, 0) : K_ERR_WAIT_TIMEOUT;
	}

	return (semaphore->count--, 0);
}