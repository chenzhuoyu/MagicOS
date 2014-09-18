/*
 * slab.c
 *
 *  Created on: 2012-11-20
 *      Author: desperedo
 *
 *	Linux-like slab allocator
 */

#include <slab.h>

#include <stdlib.h>
#include <event.h>
#include <thread.h>
#include <semaphore.h>

#define SLAB_INIT(name)				memset(k_slab_##name##_usage, 0, SLAB_MAX_CACHE_SIZE)
#define SLAB_FREE(name)				k_slab_##name##_usage[slab_get_index(type, object)] = 0
#define SLAB_CACHE(name)			size = sizeof(k_##name##_t); objects = k_slab_##name##_cache; usage = k_slab_##name##_usage

#define SLAB_INDEX(name, object)	(((ulong)(object) - (ulong)k_slab_##name##_cache) / sizeof(k_##name##_t))

#define SLAB_DEFINE(name)													\
		static uchar k_slab_##name##_usage[SLAB_MAX_CACHE_SIZE];			\
		static k_##name##_t k_slab_##name##_cache[SLAB_MAX_CACHE_SIZE]

SLAB_DEFINE(tcb);
SLAB_DEFINE(wait);
SLAB_DEFINE(event);
SLAB_DEFINE(semaphore);

ulong k_slab_usage[SLAB_CACHE_TYPES];

void init_slab()
{
	SLAB_INIT(tcb);
	SLAB_INIT(wait);
	SLAB_INIT(event);
	SLAB_INIT(semaphore);

	memset(k_slab_usage, 0, sizeof(k_slab_usage));
}

void *slab_allocate(int type)
{
	ulong size = 0;
	uchar *usage = NULL;
	void *objects = NULL;

	switch (type)
	{
		case SLAB_TCB: SLAB_CACHE(tcb); break;
		case SLAB_WAIT: SLAB_CACHE(wait); break;
		case SLAB_EVENT: SLAB_CACHE(event); break;
		case SLAB_SEMAPHORE: SLAB_CACHE(semaphore); break;

		default: return NULL;
	}

	k_slab_usage[type]++;

	for (int i = 0; i < SLAB_MAX_CACHE_SIZE; i++)
	{
		if (!usage[i])
		{
			usage[i] = 1;

			return (void *)((ulong)objects + size * i);
		}
	}

	return NULL;
}

void slab_free(int type, void *object)
{
	k_slab_usage[type]--;

	switch (type)
	{
		case SLAB_TCB:			SLAB_FREE(tcb); break;
		case SLAB_WAIT:			SLAB_FREE(wait); break;
		case SLAB_EVENT:		SLAB_FREE(event); break;
		case SLAB_SEMAPHORE:	SLAB_FREE(semaphore); break;
	}
}

int slab_get_index(int type, void *object)
{
	switch (type)
	{
		case SLAB_TCB:			return SLAB_INDEX(tcb, object);
		case SLAB_WAIT:			return SLAB_INDEX(wait, object);
		case SLAB_EVENT:		return SLAB_INDEX(event, object);
		case SLAB_SEMAPHORE:	return SLAB_INDEX(semaphore, object);
	}
}