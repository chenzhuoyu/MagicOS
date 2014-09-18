/*
 * i8259a.c
 *
 *  Created on: 2012-11-9
 *      Author: desperedo
 *
 *	i8259a interrupt controller driver
 */

#include <i8259a.h>

#include <io.h>
#include <interrupt.h>

static ushort g_mask = 0x0000;

void init_i8259a()
{
	outp(I8259_INT_M_CTL, 0x11);					// Master, ICW1
	outp(I8259_INT_S_CTL, 0x11);					// Slave, ICW1
	outp(I8259_INT_M_MASK, INT_IRQ_BASE + 0);		// Master, ICW2. Interrupt entry is set to IRQ_0
	outp(I8259_INT_S_MASK, INT_IRQ_BASE + 8);		// Slave, ICW2. Interrupt entry is set to IRQ_8
	outp(I8259_INT_M_MASK, 0x04);					// Master, ICW3. IR2 --> slave
	outp(I8259_INT_S_MASK, 0x02);					// Slave, ICW3. Slave --> IR2
	outp(I8259_INT_M_MASK, 0x01);					// Master, ICW4
	outp(I8259_INT_S_MASK, 0x01);					// Master, ICW4

	i8259_disable(I8259_IRQ_ALL);					// Disable all interrupts at first
}

void i8259_enable(ushort mask)
{
	g_mask |= mask;

	outp(I8259_INT_M_MASK, ~(g_mask & 0x00FF));
	outp(I8259_INT_S_MASK, ~((g_mask & 0xFF00) >> 8));
}

void i8259_disable(ushort mask)
{
	g_mask &= ~mask;

	outp(I8259_INT_M_MASK, ~(g_mask & 0x00FF));
	outp(I8259_INT_S_MASK, ~((g_mask & 0xFF00) >> 8));
}