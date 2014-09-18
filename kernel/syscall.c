/*
 * syscall.c
 *
 *  Created on: 2012-11-20
 *      Author: desperedo
 *
 *	syscall handlers
 */

#include <syscall.h>

#include <sched.h>
#include <errors.h>
#include <console.h>
#include <thread.h>
#include <slab.h>
#include <set.h>

static void syscall_wait();
static void syscall_exit();
static void syscall_puts();
static void syscall_setattr();
static void syscall_setxy();

static const syscall_handler_t SYSCALLS[] =
{
	syscall_wait,
	syscall_exit,
	syscall_puts,
	syscall_setattr,
	syscall_setxy
};

static inline void sys_ret()
{
	k_current_thread->context.esp += sizeof(ulong);
	k_current_thread->context.eip = *(ulong *)(k_current_thread->context.esp - sizeof(ulong));
}

static inline ulong sys_getp(ulong index)
{
	return *(ulong *)(k_current_thread->context.esp + sizeof(ulong) * (index + 1));
}

static inline void sys_setp(ulong index, ulong value)
{
	*(ulong *)(k_current_thread->context.esp + sizeof(ulong) * (index + 1)) = value;
}

static void syscall_wait()
{
	*(ulong *)sys_getp(0) = K_ERR_OK;

	k_thread_block(k_current_thread);

	sys_ret();
	sched_next();
}

static void syscall_exit()
{
	*(ulong *)sys_getp(0) = K_ERR_OK;

	k_thread_count--;

	k_current_thread->exit_code = sys_getp(1);
	list_remove(k_thread_queue[k_current_thread->state], (list_entry_t *)k_current_thread);

	k_tcb_t *tcb = k_current_thread;

	sys_ret();
	sched_next();

	slab_free(SLAB_TCB, tcb);
}

static void syscall_puts()
{
	*(ulong *)sys_getp(0) = K_ERR_OK;

	sys_setp(1, k_puts((char *)sys_getp(2)));
	sys_ret();
}

static void syscall_setattr()
{
	*(ulong *)sys_getp(0) = K_ERR_OK;

	k_setattr(LoByte(sys_getp(1)));
	sys_ret();
}

static void syscall_setxy()
{
	ulong position = sys_getp(1);

	*(ulong *)sys_getp(0) = K_ERR_OK;

	k_setxy(HiWord(position), LoWord(position));
	sys_ret();
}

void interrupt_syscall()
{
	ulong index = k_current_thread->context.eax;

	*(ulong *)sys_getp(0) = K_ERR_INVALID_SYSCALL;

	if (index >= 0 && index < sizeof(SYSCALLS) / sizeof(syscall_handler_t) && SYSCALLS[index]) SYSCALLS[index]();
}