/*
 * scan_codes.h
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	scan-code tables
 */

#ifndef __SCAN_CODES_H__
#define __SCAN_CODES_H__

#include <global.h>
#include <keyboard.h>

static char NORMAL[128] =
{
	NULL,
	VK_ESCAPE,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'-',
	'=',
	VK_BACKSPACE,
	VK_TAB,
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'[',
	']',
	VK_RETURN,
	VK_LCONTROL,
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	';',
	'\'',
	'`',
	VK_LSHIFT,
	'\\',
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',
	',',
	'.',
	'/',
	VK_RSHIFT,
	VK_MULTIPLY,
	VK_LMENU,
	VK_SPACE,
	VK_CAPSLOCK,
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_NUMLOCK,
	VK_SCROLL,
	VK_PAD_7,
	VK_PAD_8,
	VK_PAD_9,
	VK_SUBSTRACT,
	VK_PAD_4,
	VK_PAD_5,
	VK_PAD_6,
	VK_ADD,
	VK_PAD_1,
	VK_PAD_2,
	VK_PAD_3,
	VK_PAD_0,
	VK_DECIMAL,
	VK_F11,
	VK_F12
};

static char SPECIAL[128] =
{
	[0x1C] = VK_RETURN,
	[0x1D] = VK_RCONTROL,
	[0x35] = VK_DIVIDE,
	[0x38] = VK_RMENU,
	[0x47] = VK_HOME,
	[0x48] = VK_UP,
	[0x49] = VK_PAGE_UP,
	[0x4B] = VK_LEFT,
	[0x4D] = VK_RIGHT,
	[0x4F] = VK_END,
	[0x50] = VK_DOWN,
	[0x51] = VK_PAGE_DOWN,
	[0x52] = VK_INSERT,
	[0x53] = VK_DELETE,
	[0x5B] = VK_LWIN,
	[0x5C] = VK_RWIN,
	[0x5D] = VK_APPS
};

#endif /* __SCAN_CODES_H__ */