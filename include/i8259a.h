/*
 * i8259a.h
 *
 *  Created on: 2012-11-9
 *      Author: desperedo
 *
 *	i8259a interrupt controller driver
 */

#ifndef __I8259A_H__
#define __I8259A_H__

#include <global.h>

#define I8259_EOI						0x20

#define I8259_INT_M_CTL					0x20
#define I8259_INT_M_MASK				0x21
#define I8259_INT_S_CTL					0xA0
#define I8259_INT_S_MASK				0xA1

#define I8259_MASTER					0
#define I8259_SLAVE						1

// i8259A IRQs

#define I8259_IRQ_ALL					0xFFFF

// Master i8259A

#define I8259_IRQ_SYS_TICK				0x0001
#define I8259_IRQ_KEYBOARD				0x0002
#define I8259_IRQ_SLAVE					0x0004
#define I8259_IRQ_COM_2					0x0008
#define I8259_IRQ_COM_1					0x0010
#define I8259_IRQ_5						0x0020
#define I8259_IRQ_FLOPPY				0x0040
#define I8259_IRQ_LPT_1					0x0080

// Slave i8259A

#define I8259_IRQ_RTC					0x0100
#define I8259_IRQ_9						0x0200
#define I8259_IRQ_10					0x0400
#define I8259_IRQ_11					0x0800
#define I8259_IRQ_MOUSE					0x1000
#define I8259_IRQ_FPU					0x2000
#define I8259_IRQ_PRIMARY_IDE			0x4000
#define I8259_IRQ_SECONDARY_IDE			0x8000

void init_i8259a();

void i8259_enable(ushort mask);
void i8259_disable(ushort mask);

#endif /* __I8259A_H__ */