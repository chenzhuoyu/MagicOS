/*
 * keyboard.h
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	keyboard driver for Magix
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <global.h>

#define VK_BACKSPACE				0x08
#define VK_TAB						0x09
#define VK_RETURN					0x0A
#define VK_PAUSE					0x13
#define VK_CAPSLOCK					0x14
#define VK_ESCAPE					0x1B
#define VK_SPACE					0x20
#define VK_PAGE_UP					0x21
#define VK_PAGE_DOWN				0x22
#define VK_END						0x23
#define VK_HOME						0x24
#define VK_LEFT						0x25
#define VK_UP						0x26
#define VK_RIGHT					0x27
#define VK_DOWN						0x28
#define VK_SNAPSHOT					0x2C
#define VK_INSERT					0x2D
#define VK_DELETE					0x2E
#define VK_LWIN						0x5B
#define VK_RWIN						0x5C
#define VK_APPS						0x5D
#define VK_PAD_0					0x60
#define VK_PAD_1					0x61
#define VK_PAD_2					0x62
#define VK_PAD_3					0x63
#define VK_PAD_4					0x64
#define VK_PAD_5					0x65
#define VK_PAD_6					0x66
#define VK_PAD_7					0x67
#define VK_PAD_8					0x68
#define VK_PAD_9					0x69
#define VK_MULTIPLY					0x6A
#define VK_ADD						0x6B
#define VK_SUBSTRACT				0x6D
#define VK_DECIMAL					0x6E
#define VK_DIVIDE					0x6F
#define VK_F1						0x70
#define VK_F2						0x71
#define VK_F3						0x72
#define VK_F4						0x73
#define VK_F5						0x74
#define VK_F6						0x75
#define VK_F7						0x76
#define VK_F8						0x77
#define VK_F9						0x78
#define VK_F10						0x79
#define VK_F11						0x7A
#define VK_F12						0x7B
#define VK_F13						0x7C
#define VK_F14						0x7D
#define VK_F15						0x7E
#define VK_F16						0x7F
#define VK_F17						0x80
#define VK_F18						0x81
#define VK_F19						0x82
#define VK_F20						0x83
#define VK_F21						0x84
#define VK_F22						0x85
#define VK_F23						0x86
#define VK_F24						0x87
#define VK_NUMLOCK					0x90
#define VK_SCROLL					0x91
#define VK_LSHIFT					0xA0
#define VK_RSHIFT					0xA1
#define VK_LCONTROL					0xA2
#define VK_RCONTROL					0xA3
#define VK_LMENU					0xA4
#define VK_RMENU					0xA5

void init_keyboard();

void keyboard_read_key(uchar *make, uchar *buffer);
void keyboard_read_modifier(uchar *ctrl, uchar *alt, uchar *shift);

void interrupt_keyboard();

#endif /* __KEYBOARD_H__ */