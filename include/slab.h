/*
 * slab.h
 *
 *  Created on: 2012-11-20
 *      Author: desperedo
 */

#ifndef __SLAB_H__
#define __SLAB_H__

#include <global.h>

#define SLAB_CACHE_TYPES		4
#define SLAB_MAX_CACHE_SIZE		1024

#define SLAB_TCB				0
#define SLAB_WAIT				1
#define SLAB_EVENT				2
#define SLAB_SEMAPHORE			3

extern ulong k_slab_usage[SLAB_CACHE_TYPES];

void init_slab();

void *slab_allocate(int type);
void slab_free(int type, void *object);

int slab_get_index(int type, void *object);

#endif /* __SLAB_H__ */