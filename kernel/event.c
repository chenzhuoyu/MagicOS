/*
 * event.c
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	event for thread synchronization
 */

#include <event.h>

#include <slab.h>
#include <thread.h>
#include <errors.h>
#include <stdlib.h>
#include <ki.h>

k_event_t *k_event_new()
{
	k_event_t *event = slab_allocate(SLAB_EVENT);

	event->wait = 0;
	event->state = 0;

	return event;
}

void k_event_free(k_event_t *event)
{
	if (event->wait)
	{
		ulong result;

		k_event_signal(event);
		ki_wait(&result);
	}

	slab_free(SLAB_EVENT, event);
}

void k_event_signal(k_event_t *event)
{
	cli();
	event->state = 1;
	sti();
}

uchar k_event_wait(k_event_t *event, ulong timeout)
{
	if (event->state == 0)
	{
		ulong result;
		k_wait_t *wait = slab_allocate(SLAB_WAIT);

		wait->object = event;
		wait->timeout = timeout;

		cli();

		event->wait++;

		set_set(&k_current_thread->wait, WAIT_EVENTS);
		list_append(&k_current_thread->events, (list_entry_t *)wait);

		sti();

		ki_wait(&result);

		return event->state ? (event->state = 0) : K_ERR_WAIT_TIMEOUT;
	}

	return event->state = 0;
}