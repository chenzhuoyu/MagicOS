/*
 * keyboard.c
 *
 *  Created on: 2012-12-10
 *      Author: desperedo
 *
 *	keyboard driver for Magix
 */

#include <keyboard.h>

#include <io.h>
#include <thread.h>
#include <interrupt.h>
#include <scan_codes.h>
#include <semaphore.h>
#include <console.h>

#define S2V(s)			(g_ext_flag == 0 ? NORMAL[(s) & 0x7F] : (g_ext_flag = 0, SPECIAL[(s) & 0x7F]))

static char g_discard = 0;
static char g_ext_flag = 0x00;

static uchar g_num_lock = 0;
static uchar g_caps_lock = 0;

static uchar g_modifier_alt = 0;
static uchar g_modifier_ctrl = 0;
static uchar g_modifier_shift = 0;

static int g_text_queue_head = 0;
static int g_text_queue_tail = 1;

static int g_input_queue_head = 0;
static int g_input_queue_tail = 1;

static uchar g_text_make[128] = {0x00};
static uchar g_text_buffer[128] = {0x00};

static uchar g_input_make[128] = {0x00};
static uchar g_input_buffer[128] = {0x00};

static k_semaphore_t *g_text_event = NULL;
static k_semaphore_t *g_input_event = NULL;

static inline void text_queue_put(uchar make, uchar buffer)
{
	g_text_make[g_text_queue_tail] = make;
	g_text_buffer[g_text_queue_tail] = buffer;

	g_text_queue_tail = (g_text_queue_tail + 1) % 128;
}

static inline void text_queue_get(uchar *make, uchar *buffer)
{
	g_text_queue_head = (g_text_queue_head + 1) % 128;

	*make = g_text_make[g_text_queue_head];
	*buffer = g_text_buffer[g_text_queue_head];
}

static inline void input_queue_put(uchar make, uchar buffer)
{
	g_input_make[g_input_queue_tail] = make;
	g_input_buffer[g_input_queue_tail] = buffer;

	g_input_queue_tail = (g_input_queue_tail + 1) % 128;
}

static inline void input_queue_get(uchar *make, uchar *buffer)
{
	g_input_queue_head = (g_input_queue_head + 1) % 128;

	*make = g_input_make[g_input_queue_head];
	*buffer = g_input_buffer[g_input_queue_head];
}

static int keyboardd(void *param)
{
	for (;;)
	{
		uchar make;
		uchar buffer;

		k_sem_wait(g_input_event, INFINITE);
		input_queue_get(&make, &buffer);

		switch (buffer)
		{
			case VK_NUMLOCK: g_num_lock ^= !make; break;
			case VK_CAPSLOCK: g_caps_lock ^= !make; break;

			case VK_LMENU:		case VK_RMENU:		g_modifier_alt = make; break;
			case VK_LCONTROL:	case VK_RCONTROL:	g_modifier_ctrl = make; break;
			case VK_LSHIFT:		case VK_RSHIFT:		g_modifier_shift = make; break;

			default:
			{
				switch (buffer)
				{
					case VK_ADD:		buffer = '+'; break;
					case VK_SUBSTRACT:	buffer = '-'; break;
					case VK_MULTIPLY:	buffer = '*'; break;
					case VK_DIVIDE:		buffer = '/'; break;

					case VK_PAD_0:		buffer = g_num_lock ? '0' : VK_INSERT;		break;
					case VK_PAD_1:		buffer = g_num_lock ? '1' : VK_END;			break;
					case VK_PAD_2:		buffer = g_num_lock ? '2' : VK_DOWN;		break;
					case VK_PAD_3:		buffer = g_num_lock ? '3' : VK_PAGE_DOWN;	break;
					case VK_PAD_4:		buffer = g_num_lock ? '4' : VK_LEFT;		break;
					case VK_PAD_5:		buffer = g_num_lock ? '5' : 0;				break;
					case VK_PAD_6:		buffer = g_num_lock ? '6' : VK_RIGHT;		break;
					case VK_PAD_7:		buffer = g_num_lock ? '7' : VK_HOME;		break;
					case VK_PAD_8:		buffer = g_num_lock ? '8' : VK_UP;			break;
					case VK_PAD_9:		buffer = g_num_lock ? '9' : VK_PAGE_UP;		break;
					case VK_DECIMAL:	buffer = g_num_lock ? '.' : VK_DELETE;		break;

					default: if (buffer >= 'A' && buffer <= 'Z' && g_caps_lock == g_modifier_shift) buffer += 'a' - 'A';
				}

				if (buffer)
				{
					text_queue_put(make, buffer);

					k_sem_signal(g_text_event);
				}

				break;
			}
		}
	}

	return 0;
}

void init_keyboard()
{
	g_text_event = k_sem_new(INFINITE);
	g_input_event = k_sem_new(INFINITE);

	k_sem_none(g_text_event);
	k_sem_none(g_input_event);

	k_thread_new(keyboardd, NULL);
}

void keyboard_read_key(uchar *make, uchar *buffer)
{
	k_sem_wait(g_text_event, INFINITE);

	text_queue_get(make, buffer);
}

void keyboard_read_modifier(uchar *ctrl, uchar *alt, uchar *shift)
{
	*alt = g_modifier_alt;
	*ctrl = g_modifier_ctrl;
	*shift = g_modifier_shift;
}

void interrupt_keyboard()
{
	char scan = inp(0x60);

	if (g_discard)
	{
		g_discard--;
	}
	else if (scan == 0xE0)
	{
		g_ext_flag = scan;
	}
	else if (scan == 0xE1)
	{
		g_discard = 5;

		input_queue_put(1, VK_PAUSE);
	}
	else
	{
		input_queue_put((scan & 0x80) == 0, S2V(scan));

		k_sem_signal(g_input_event);
	}
}