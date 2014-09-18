/*
 * i8042.h
 *
 *  Created on: 2012-11-16
 *      Author: desperedo
 *
 *	intel 8042 keyboard controller driver
 */

#ifndef __I8042_H__
#define __I8042_H__

#include <global.h>

#define I8042_PORT_DATA			0x60
#define I8042_PORT_COMMAND		0x64

void init_i8042();

#endif /* __I8042_H__ */