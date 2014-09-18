/*
 * interrupt.c
 *
 *  Created on: 2012-11-9
 *      Author: desperedo
 *
 *	interrupt handlers
 */

#include <interrupt.h>

#include <colors.h>
#include <console.h>
#include <i8259a.h>
#include <thread.h>
#include <sched.h>
#include <syscall.h>
#include <keyboard.h>
#include <int_vectors.h>

ulong volatile k_sys_tick = 0;
ulong volatile k_int_nested = 0;

static const char *ERROR_MESSAGES[] =
{
	"#DE -- Divide exception",
	"#DB -- Debug exception",
	"... -- NMI interrupt",
	"#BP -- Breakpoint",
	"#OF -- Overflow exception",
	"#BR -- Bound range exceeded",
	"#UD -- Invalid Opcode",
	"#NM -- Device not available",
	"#DF -- Double fault",
	"... -- Reserved",
	"#TS -- Invalid TSS",
	"#NP -- Segment not present",
	"#SS -- Stack segment fault",
	"#GP -- Generic protection",
	"#PF -- Page fault",
	"... -- Intel reserved",
	"#MF -- x87 FPU math fault",
	"#AC -- Align check fault",
	"#MC -- Machine check fault"
};

void init_interrupt()
{
	init_i8259a();

	i8259_enable(I8259_IRQ_SYS_TICK);
	i8259_enable(I8259_IRQ_KEYBOARD);

	for (int i = 0; i < sizeof(INTERRUPT_VECTORS) / sizeof(int_vector_t); i++)
	{
		const int_vector_t *vector = &INTERRUPT_VECTORS[i];

		init_idt_item(vector->vector, K_DA_TYPE_386_INTERRUPT_GATE, vector->handler, vector->dpl);
	}
}

// Generic exception handler

static void generic_exception_handler(ulong vector)
{
	k_tcb_t *current = k_current_thread;

	k_setattr(LIGHT_RED);
	k_printf("\n");
	k_printf("VECTOR : %#.2X\n", vector);
	k_printf("EXCEPTION  : %s\n", ERROR_MESSAGES[vector]);
	k_printf("ERROR CODE : %#.8X\n\n", current->exception);
	k_printf("eip = %#.8X , cs = %#.8X , eflags = %#.8X\n", current->context.eip, current->context.cs, current->context.eflags);

	cli();

	for (;;);

	list_remove((linked_list_t *)&k_thread_ready_list, (list_entry_t *)k_current_thread);

	sched_switch();
}

// Generic interrupt handler

void generic_interrupt_handler(ulong vector)
{
	if (vector < INT_EXCEPTION_COUNT)
	{
		generic_exception_handler(vector);
	}
	else
	{
		switch (vector)
		{
			case INT_IRQ_BASE + 0:		interrupt_sys_tick(); break;
			case INT_IRQ_BASE + 1:		interrupt_keyboard(); break;

			case INT_IRQ_SYSCALL:		interrupt_syscall(); break;
		}
	}
}