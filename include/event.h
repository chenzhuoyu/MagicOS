/*
 * event.h
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	event for thread synchronization
 */

#ifndef __EVENT_H__
#define __EVENT_H__

#include <global.h>
#include <linked_list.h>

typedef struct _k_event_t
{
	list_entry_t	list;
	ulong			wait;
	uchar			state;
} k_event_t;

k_event_t *k_event_new();
void k_event_free(k_event_t *event);

void k_event_signal(k_event_t *event);
uchar k_event_wait(k_event_t *event, ulong timeout);

#endif /* __EVENT_H__ */