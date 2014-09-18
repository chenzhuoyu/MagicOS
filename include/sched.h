/*
 * sched.h
 *
 *  Created on: 2012-11-19
 *      Author: desperedo
 *
 *	process and thread scheduler
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <global.h>

void sched_start();

void sched_next();
void sched_switch();

void interrupt_sys_tick();

#endif /* __SCHED_H__ */