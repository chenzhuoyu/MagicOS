/*
 * linked_list.h
 *
 *  Created on: 2012-11-19
 *      Author: desperedo
 *
 *	two-way linked list
 */

#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <global.h>
#include <slab.h>

typedef struct _list_entry_t
{
	struct _list_entry_t	*prev;
	struct _list_entry_t	*next;
} list_entry_t;

typedef struct _linked_list_t
{
	ulong node_count;

	list_entry_t *head;
	list_entry_t *tail;
} linked_list_t;

static inline void list_init(linked_list_t *list)
{
	list->node_count = 0;

	list->tail = list->head = NULL;
}

static inline uchar list_empty(linked_list_t *list)
{
	return list->node_count == 0;
}

static inline void list_remove(linked_list_t *list, list_entry_t *node)
{
	if (list->node_count)
	{
		list->node_count--;

		if (node == list->tail) list->tail = list->tail->prev;
		if (node == list->head) list->head = list->head->next;

		if (node->next) node->next->prev = node->prev;
		if (node->prev) node->prev->next = node->next;

		node->prev = node->next = NULL;
	}
}

static inline void list_append(linked_list_t *list, list_entry_t *node)
{
	list->node_count++;

	if (list->head == NULL)
	{
		node->prev = node->next = NULL;

		list->tail = list->head = node;
	}
	else
	{
		node->next = NULL;
		node->prev = list->tail;

		list->tail->next = node;
		list->tail = node;
	}
}

static inline void list_walk(linked_list_t *list, uchar (* walker)(linked_list_t *list, list_entry_t *item))
{
	uchar result = 1;
	list_entry_t *node = list->head;

	while (node && result)
	{
		list_entry_t *next = node->next;

		result = walker(list, node);
		node = next;
	}
}

#endif /* __LINKED_LIST_H__ */