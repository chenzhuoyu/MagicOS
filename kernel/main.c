/*
 * main.c
 *
 *  Created on: 2012-11-7
 *      Author: desperedo
 *
 *	Magix kernel
 */

#include <stdlib.h>
#include <colors.h>
#include <console.h>
#include <interrupt.h>
#include <i8042.h>
#include <keyboard.h>
#include <mm.h>
#include <slab.h>
#include <thread.h>
#include <sched.h>
#include <errors.h>
#include <event.h>
#include <ui.h>

static int sys_idle(void *param)
{
	for (;;) nop();

	return 0;
}

void main()
{
	k_setxy(0, 16);

	init_interrupt();
	init_i8042();

	init_mm();
	init_slab();
	init_thread();
	init_keyboard();

	k_thread_new(sys_idle, NULL);

	ui_startup();

	sched_start(); // This function should never returns
}