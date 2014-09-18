/*
 * sched.c
 *
 *  Created on: 2012-11-19
 *      Author: desperedo
 *
 *	process and thread scheduler
 */

#include <sched.h>

#include <thread.h>
#include <syscall.h>
#include <interrupt.h>
#include <event.h>
#include <semaphore.h>
#include <set.h>

static uchar walk_event_list(linked_list_t *list, list_entry_t *item)
{
	k_wait_t *wait = (k_wait_t *)item;
	k_event_t *event = (k_event_t *)wait->object;

	if (event->state || wait->timeout-- == 0)
	{
		event->wait--;

		list_remove(list, item);

		slab_free(SLAB_WAIT, item);
	}

	return 1;
}

static uchar walk_semaphore_list(linked_list_t *list, list_entry_t *item)
{
	k_wait_t *wait = (k_wait_t *)item;
	k_semaphore_t *semaphore = (k_semaphore_t *)wait->object;

	if (semaphore->count || wait->timeout-- == 0)
	{
		semaphore->wait--;

		list_remove(list, item);

		slab_free(SLAB_WAIT, item);
	}

	return 1;
}

static uchar walk_block_list(linked_list_t *list, list_entry_t *item)
{
	k_tcb_t *tcb = (k_tcb_t *)item;

	if (set_test(&tcb->wait, WAIT_TICKS))
	{
		tcb->sleep_ticks--;

		if (tcb->sleep_ticks == 0) set_clear(&tcb->wait, WAIT_TICKS);
	}

	if (set_test(&tcb->wait, WAIT_EVENTS))
	{
		list_walk(&tcb->events, walk_event_list);

		if (list_empty(&tcb->events)) set_clear(&tcb->wait, WAIT_EVENTS);
	}

	if (set_test(&tcb->wait, WAIT_SEMAPHORES))
	{
		list_walk(&tcb->semaphores, walk_semaphore_list);

		if (list_empty(&tcb->semaphores)) set_clear(&tcb->wait, WAIT_SEMAPHORES);
	}

	if (set_empty(&tcb->wait)) k_thread_ready(tcb);

	return 1;
}

void sched_next()
{
	k_current_thread->state = THREAD_STATE_READY;
	k_current_thread = (k_tcb_t *)k_current_thread->list.next;

	if (k_current_thread == NULL) k_current_thread = (k_tcb_t *)k_thread_ready_list.head;

	k_current_thread->state = THREAD_STATE_RUNNING;
}

void sched_switch()
{
	list_walk((linked_list_t *)&k_thread_block_list, walk_block_list);

	sched_next();
}

void interrupt_sys_tick()
{
	k_sys_tick++;

	if (!k_int_nested) sched_switch();
}