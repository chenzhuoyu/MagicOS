/*
 * console.c
 *
 *  Created on: 2012-11-7
 *      Author: desperedo
 *
 *	text mode video driver for Magix
 */

#include <console.h>

#include <io.h>
#include <colors.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <keyboard.h>

static char g_print_buffer[1024] = {0};

static int g_display_ptr = 0;
static uchar g_display_attr = MAKE_COLOR(WHITE, BLACK);

ushort * const k_video_mem = (ushort *)0x000B8000;

static void set_cursor(int addr)
{
	outp(PORT_VIDEO_CMD, 14);
	outp(PORT_VIDEO_DATA, (addr >> 8) & 0xFF);
	outp(PORT_VIDEO_CMD, 15);
	outp(PORT_VIDEO_DATA, addr & 0xFF);
}

static void scroll_up()
{
	g_display_ptr -= 80;

	ushort *ptr = k_video_mem + g_display_ptr;

	memcpy(k_video_mem, k_video_mem + 80, 80 * 24 * 2);

	while (ptr < k_video_mem + 80 * 25) *ptr++ = g_display_attr << 8;
}

void k_setattr(uchar attr)
{
	g_display_attr = attr;
}

void k_setxy(int x, int y)
{
	set_cursor(g_display_ptr = x + y * 80);
}

void k_cls(uchar attr)
{
	k_setxy(0, 0);

	for (ushort *ptr = k_video_mem; ptr < k_video_mem + 80 * 25; ptr++) *ptr = attr << 8;
}

int k_getch()
{
	uchar make = 0;
	char buffer = 0;

	while (!make) keyboard_read_key(&make, &buffer);

	return buffer;
}


int k_getche()
{
	uchar make = 0;
	char buffer = 0;

	while (!make) keyboard_read_key(&make, &buffer);

	k_putchar(buffer);

	return buffer;
}

int k_gets(char *s)
{
	int n = 0;
	char c = 0;

	while ((c = k_getch()) != VK_RETURN)
	{
		switch (c)
		{
			case VK_BACKSPACE:
			{
				if (n > 0)
				{
					n--;

					set_cursor(--g_display_ptr);

					k_video_mem[g_display_ptr] = g_display_attr << 8;
				}

				break;
			}

			default:
			{
				if (c > 0x5D && c < 0x60)
				{
					s[n++] = c;

					k_putchar(c);
				}

				break;
			}
		}
	}

	s[n] = 0;

	k_putchar(VK_RETURN);

	return n;
}

int k_putchar(int c)
{
	switch (c)
	{
		case '\r':
		case '\n':
		{
			g_display_ptr /= 80;
			g_display_ptr *= 80;

			if (c == '\n') g_display_ptr += 80;

			set_cursor(g_display_ptr);

			break;
		}

		default:
		{
			k_video_mem[g_display_ptr++] = c | (g_display_attr << 8);

			break;
		}
	}

	if (g_display_ptr >= 80 * 25) scroll_up();

	set_cursor(g_display_ptr);

	return c;
}

int k_puts(const char *s)
{
	const char *ptr = s;

	while (*ptr) k_putchar((uchar)*ptr++);

	return ptr - s;
}

int k_printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsprintf(g_print_buffer, fmt, args);
	va_end(args);

	return k_puts(g_print_buffer);
}