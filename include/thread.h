/*
 * thread.h
 *
 *  Created on: 2012-11-18
 *      Author: desperedo
 *
 *	thread management
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <global.h>
#include <linked_list.h>
#include <set.h>

#define THREAD_MAX_COUNT			256

#define THREAD_LDT_SIZE				2
#define THREAD_STACK_SIZE			2057

#define THREAD_STATE_COUNT			3

#define THREAD_STATE_RUNNING		0
#define THREAD_STATE_READY			1
#define THREAD_STATE_BLOCKING		2

#define WAIT_TICKS					0
#define WAIT_EVENTS					1
#define WAIT_SEMAPHORES				2

// Thread function

typedef int (* thread_fn_t)(void *context);

// Thread context

typedef struct _k_context_t
{
	ulong	gs;
	ulong	fs;
	ulong	es;
	ulong	ds;
	ulong	edi;
	ulong	esi;
	ulong	ebp;
	ulong	esp_k;
	ulong	ebx;
	ulong	edx;
	ulong	ecx;
	ulong	eax;
	ulong	ret_addr;
	ulong	eip;
	ulong	cs;
	ulong	eflags;
	ulong	esp;
	ulong	ss;
} k_context_t;

typedef struct _k_fpu_context_t
{
	ushort	fcw;
	ushort	fsw;
	ushort	ftw;
	ushort	fop;
	ulong	ip;
	ushort	cs;
	ushort	rsvd1;
	ulong	dp;
	ushort	ds;
	ushort	rsvd2;
	ulong	mxcsr;
	ulong	rsvd3;
	uchar	st0[16];
	uchar	st1[16];
	uchar	st2[16];
	uchar	st3[16];
	uchar	st4[16];
	uchar	st5[16];
	uchar	st6[16];
	uchar	st7[16];
	uchar	xmm0[16];
	uchar	xmm1[16];
	uchar	xmm2[16];
	uchar	xmm3[16];
	uchar	xmm4[16];
	uchar	xmm5[16];
	uchar	xmm6[16];
	uchar	xmm7[16];
	uchar	rsvd4[224];
} k_fpu_context_t;

// Thread wait data

typedef struct _k_wait_t
{
	list_entry_t	list;
	ulong			timeout;
	void			*object;
} k_wait_t;

// Thread control block, MUST aligned with 16 bytes !!!

typedef struct _k_tcb_t
{
	list_entry_t		list;

	k_context_t			context;
	k_fpu_context_t		fpu_context;
	ulong				exception;

	ushort				ldt_selector;
	k_gdt_ldt_t			ldt[THREAD_LDT_SIZE];

	uchar				state;

	void *				param;
	int					exit_code;
	thread_fn_t			thread_fn;

	ulong				tid;
	ulong				rpl;
	ulong				sleep_ticks;

	set_t				wait;
	linked_list_t		events;
	linked_list_t		semaphores;

	uchar				stack[THREAD_STACK_SIZE];
} k_tcb_t;

// Task status segment

typedef struct _k_tss_t
{
	ulong		backlink;
	ulong		esp0;
	ulong		ss0;
	ulong		esp1;
	ulong		ss1;
	ulong		esp2;
	ulong		ss2;
	ulong		cr3;
	ulong		eip;
	ulong		eflags;
	ulong		eax;
	ulong		ecx;
	ulong		edx;
	ulong		ebx;
	ulong		esp;
	ulong		ebp;
	ulong		esi;
	ulong		edi;
	ulong		es;
	ulong		cs;
	ulong		ss;
	ulong		ds;
	ulong		fs;
	ulong		gs;
	ulong		ldt;
	ushort		trap;
	ushort		io_bitmap;
} k_tss_t;

extern int k_thread_count;

extern k_tss_t k_tss;
extern k_tcb_t *k_current_thread;

extern linked_list_t k_thread_block_list;
extern linked_list_t k_thread_ready_list;

extern linked_list_t *k_thread_queue[THREAD_STATE_COUNT];

void init_thread();

void thread_yield();
void thread_sleep(ulong ticks);
void thread_exit(ulong exit_code);

void k_thread_block(k_tcb_t *tcb);
void k_thread_ready(k_tcb_t *tcb);

void k_thread_kill(k_tcb_t *tcb, ulong exit_code);

k_tcb_t *k_thread_new(thread_fn_t thread_fn, void *param);
k_tcb_t *k_thread_new_3(thread_fn_t thread_fn, void *param);

#endif /* __THREAD_H__ */