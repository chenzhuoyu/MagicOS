/*
 * thread.c
 *
 *  Created on: 2012-11-18
 *      Author: desperedo
 *
 *	thread management
 */

#include <thread.h>

#include <stdlib.h>
#include <slab.h>
#include <sched.h>
#include <ki.h>

static ulong g_thread_id = 0;

int k_thread_count = 0;

k_tss_t k_tss;
k_tcb_t *k_current_thread = NULL;

linked_list_t k_thread_block_list;
linked_list_t k_thread_ready_list;

linked_list_t *k_thread_queue[THREAD_STATE_COUNT] = { &k_thread_ready_list, &k_thread_ready_list, &k_thread_block_list };

static void ki_thread_loader()
{
	k_tcb_t *current = k_current_thread;

	thread_exit(current->thread_fn(current->param));
}

static void ki_thread_new(k_tcb_t *tcb, int tid, uchar rpl, thread_fn_t thread_fn, void *param)
{
	int id = slab_get_index(SLAB_TCB, tcb);

	init_gdt_ldt_item(
			K_INDEX_OF(K_GDTS_LDT_FIRST) + id,
			get_physical_addr(K_GDTS_FLAT_RW, (ulong)tcb->ldt),
			sizeof(tcb->ldt),
			K_DA_TYPE_LDT);

	tcb->tid = tid;
	tcb->rpl = rpl;

	tcb->sleep_ticks = 0;

	list_init(&tcb->events);

	tcb->param = param;
	tcb->thread_fn = thread_fn;

	tcb->state = THREAD_STATE_READY;

	tcb->ldt_selector = K_GDTS_LDT_FIRST + (id << 3);

	memcpy(&tcb->ldt[0], &k_gdt[K_INDEX_OF(K_GDTS_FLAT_X)], sizeof(k_gdt_ldt_t));
	memcpy(&tcb->ldt[1], &k_gdt[K_INDEX_OF(K_GDTS_FLAT_RW)], sizeof(k_gdt_ldt_t));

	tcb->ldt[0].attr_1 = (rpl << 5) | K_DA_SEG_X;
	tcb->ldt[1].attr_1 = (rpl << 5) | K_DA_SEG_RW;

	memset(&tcb->context, 0, sizeof(k_context_t));
	memset(&tcb->fpu_context, 0, sizeof(k_fpu_context_t));

	tcb->context.gs = K_GDTS_VIDEO;

	tcb->context.cs = K_SELECTOR(0, rpl | K_SA_TI_LDT);
	tcb->context.ds = K_SELECTOR(1, rpl | K_SA_TI_LDT);
	tcb->context.es = K_SELECTOR(1, rpl | K_SA_TI_LDT);
	tcb->context.fs = K_SELECTOR(1, rpl | K_SA_TI_LDT);
	tcb->context.ss = K_SELECTOR(1, rpl | K_SA_TI_LDT);

	tcb->context.eip = (ulong)ki_thread_loader;
	tcb->context.esp = (ulong)tcb->stack + THREAD_STACK_SIZE;

	tcb->context.eflags = 0x00001202;

	cli();

	k_thread_count++;

	list_append((linked_list_t *)&k_thread_ready_list, (list_entry_t *)tcb);

	sti();

	if (k_current_thread == NULL)
	{
		k_current_thread = (k_tcb_t *)k_thread_ready_list.head;

		k_current_thread->state = THREAD_STATE_RUNNING;
	}
}

void init_thread()
{
	memset(&k_tss, 0, sizeof(k_tss_t));

	list_init(&k_thread_block_list);
	list_init(&k_thread_ready_list);

	k_tss.ss0 = K_GDTS_FLAT_RW;
	k_tss.io_bitmap = sizeof(k_tss_t) + 1;

	init_gdt_ldt_item(
			K_INDEX_OF(K_GDTS_TSS),
			get_physical_addr(K_GDTS_FLAT_RW, (ulong)&k_tss),
			sizeof(k_tss_t),
			K_DA_TYPE_386_TSS);

	__asm__ volatile
	(
		"ltr	%%ax"		"\r\n"
		:
		: "a"(K_GDTS_TSS)
		:
	);
}

void thread_yield()
{
	ulong result;

	ki_wait(&result);
}

void thread_sleep(ulong ticks)
{
	if (ticks)
	{
		ulong result;

		cli();

		k_current_thread->sleep_ticks = ticks;
		set_set(&k_current_thread->wait, WAIT_TICKS);

		sti();

		ki_wait(&result);
	}
}

void thread_exit(ulong exit_code)
{
	ulong result;

	ki_exit(&result, exit_code);
}

void k_thread_block(k_tcb_t *tcb)
{
	cli();

	tcb->state = THREAD_STATE_BLOCKING;

	list_remove((linked_list_t *)&k_thread_ready_list, (list_entry_t *)tcb);
	list_append((linked_list_t *)&k_thread_block_list, (list_entry_t *)tcb);

	sti();
}

void k_thread_ready(k_tcb_t *tcb)
{
	cli();

	tcb->state = THREAD_STATE_READY;

	list_remove((linked_list_t *)&k_thread_block_list, (list_entry_t *)tcb);
	list_append((linked_list_t *)&k_thread_ready_list, (list_entry_t *)tcb);

	sti();
}

void k_thread_kill(k_tcb_t *tcb, ulong exit_code)
{
	if (tcb == k_current_thread) thread_exit(exit_code);

	tcb->context.eip = (ulong)thread_exit;
	*(ulong *)tcb->context.esp = exit_code;

	if (tcb->state == THREAD_STATE_BLOCKING) k_thread_ready(tcb);
}

k_tcb_t *k_thread_new(thread_fn_t thread_fn, void *param)
{
	k_tcb_t *tcb = slab_allocate(SLAB_TCB);

	ki_thread_new(tcb, g_thread_id++, K_SA_RPL_1, thread_fn, param);

	return tcb;
}

k_tcb_t *k_thread_new_3(thread_fn_t thread_fn, void *param)
{
	k_tcb_t *tcb = slab_allocate(SLAB_TCB);

	ki_thread_new(tcb, g_thread_id++, K_SA_RPL_3, thread_fn, param);

	return tcb;
}