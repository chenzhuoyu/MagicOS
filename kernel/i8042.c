/*
 * i8042.c
 *
 *  Created on: 2012-11-16
 *      Author: desperedo
 *
 *	intel 8042 keyboard controller driver
 */

#include <i8042.h>

#include <io.h>

void init_i8042()
{
	outp(I8042_PORT_COMMAND, 0x60);					// 0x60 : write Controller Command Byte
	outp(I8042_PORT_DATA, 0x4B);
}